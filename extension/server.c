#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <ifaddrs.h>
#include <ncurses.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>
#include "structs.h"
#include "server.h"
#include "logic.h"
#include "config.h"

extern int list_s;
extern char **global_ips;
extern int global_ip_num;
extern Direction upDir;
extern Direction leftDir;
extern Direction rightDir;
extern Direction downDir;
extern Direction noDir;

Request parseRequest(int sock) {
    FILE *fileStream;
    fileStream = fdopen(sock, "r");
    Request request;
    request.html = false;
    request.css = false;
    request.img = false;
    request.dir = 0;
    size_t size = 1;
    char *msg = malloc(sizeof(char));
    msg[0] = '\0';
    size = 1;
    char buff[2000];
    int count = 0;
    int lim = 0;

    char *file = malloc(sizeof(char) * 30);
    char *cont = malloc(sizeof(char) * 40);
    int length = 10;
    while (1) {
        if (!fgets(buff, 2000, fileStream)) {
            puts("Read failed");
            break;
        }

        if(!strcmp(buff, "\r\n")){
            while(count < lim){
                fgets(buff, length + 1, fileStream);
                size += strlen(buff);
                msg = realloc(msg, size);
                strcat(msg, buff);
                count++;
                char* temp = malloc(sizeof(char) * (1 + strlen(buff)));
                strcpy(temp, buff);
                request.dir = temp;
            }
            break;
        }

        if(strstr(buff, ".jpg") != NULL){
            //puts("img");
            request.img = true;
        } else if(strstr(buff, ".html") != NULL){
            //puts("html");
            request.html = true;
        } else if(strstr(buff, ".css") != NULL){
            request.css = true;
            //puts("css");
        }

        if(strstr(buff, "GET") != NULL){
            sscanf(buff, "GET %s HTTP/1.1", file);
            //printf("Get request for %s\n", file);
            request.get = true;
        }
        if(strstr(buff, "POST") != NULL){
            sscanf(buff, "POST %s HTTP/1.1", file);
            //printf("Post request for %s\n", file);
            lim++;
            request.get = false;
        }
        if(strstr(buff, "Content-Length:") != NULL){
            sscanf(buff, "Content-Length: %s", cont);
            length = atoi(cont);
        }



        size += strlen(buff);
        msg = realloc(msg, size);
        strcat(msg, buff);
    }
    //puts(msg);
    request.msg = msg;
    request.file = file;
    //puts(msg);
    return request;

}

void writeFile(Request request, int soc) {
    char *str = "HTTP/1.0 200 OK\nServer: CS241Serv v0.1\nContent-Type: text/html\n\n";
    char *openCond = "r";
    if(request.css){
        str = "HTTP/1.0 200 OK\nServer: CS241Serv v0.1\nContent-Type: text/css\n\n";
    }
    if(request.img){
        openCond = "rb";
        str = "HTTP/1.0 200 OK\nServer: CS241Serv v0.1\nContent-Type: image/jpeg\n\n";
    }
    FILE *fp;
    char *file =malloc(sizeof(char) * (strlen(request.file) + strlen("\\webapp")));
    strcpy(file, "webapp");
    strcat(file, request.file);
    fp = fopen(file, openCond);
    if(fp == NULL){
        printf("Failed to open file %s\n", file);
        return;
    }

    write(soc ,str, strlen(str));
    fseek (fp, 0, SEEK_END);
    long len = ftell(fp);
    rewind(fp);
    char buff[len];
    fread(buff, 1, len, fp);
    write(soc, buff, len);
    fclose(fp);
}

int getIP(const char *ip, Game *game) {
    //gets index of IP or adds it if it doesn't exist yet
    //returns -1 if too many players
    for (int i = 0; i < global_ip_num; ++i) {
        if (!strcmp(global_ips[i], ip)) {
            return i + 1;
        }
    }
    //IP doesn't exit yet
    if (global_ip_num < game->players) {
        char *newip = malloc(sizeof(char) * (strlen(ip) + 1));
        strcpy(newip, ip);
        global_ips[global_ip_num] = newip;
        global_ip_num++;
        return global_ip_num;
    } else {
        return -1;
    }
}

void returnFile(Request request, int con, Game *game) {
    if(!strcmp(request.file, "/")){
        int n = getIP(request.ip, game);
        if (n == -1){
            strcpy(request.file, "/err.html");
            writeFile(request, con);
        } else {
            request.html = true;
            request.css = false;
            request.img = false;
            char *site = malloc(sizeof(char) * 13);
            strcpy(site, "/webapp");
            char nr[2];
            nr[1] = '\0';
            nr[0] = (char) (n + '0');
            strcat(site, nr);
            strcat(site, ".html");
            strcpy(request.file, site);
            writeFile(request, con);
            free(site);
        }
    } else {
        writeFile(request, con);
    }
}

void clean(int sig) {
    puts("\nCleaning");
    close(list_s);
    exit(EXIT_SUCCESS);
}

void processCommand(Game *game, Request request) {
    if (game->started && request.dir) {
        char *dir = strstr(request.dir, "=");
        dir++;
        Direction next;
        if (!strcmp(dir, "Up")) {
            next = upDir;
        } else if (!strcmp(dir, "Right")) {
            next = rightDir;
        } else if (!strcmp(dir, "Down")) {
            next = downDir;
        } else {
            next = leftDir;
        }
        for (int i = 0; i < global_ip_num; ++i) {
            if (!strcmp(global_ips[i], request.ip)) {
                if (!oppositeDir(game->snakes[game->noOfSnakes - i - 1], next)) {
                    game->snakes[game->noOfSnakes - i - 1]->nextDir = next;
                }
            }
        }
    }
    free(request.dir);
    free(request.file);
    free(request.msg);
}

void *processPosts(void *ptr) {
    //Server setup
    ServerUtils *u = (ServerUtils *) ptr;
    Game *game = u->game;
    struct sockaddr_in *serverAddress = u->serverAddress;
    int *addrSize = u->addrSize;

    while (!game->finished) {
        //Processes a post request
        int conn_s = accept(list_s, (struct sockaddr *) serverAddress,
                            (socklen_t *) addrSize);
        if (conn_s == -1) {
            puts("Error handling connection");
        }
        Request request = parseRequest(conn_s);
        request.ip = inet_ntoa(serverAddress->sin_addr);
        returnFile(request, conn_s, game);
        processCommand(game, request);
        close(conn_s);
    }
    return NULL;
}

void waitForConnections(ServerUtils *u) {
    clear();
    int ch;
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char *addr = NULL;
    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            if (strcmp(ifa->ifa_name, "lo") != 0) {
                addr = inet_ntoa(sa->sin_addr);
            }
        }
    }
    freeifaddrs(ifap);
    while((ch = getch()) != 10) {
        if (ch == 'x') {
            endwin();
            exit(EXIT_FAILURE);
        }
        move(0,0);
        if (addr != NULL) {
            printw("Connect to: %s:%d", addr, PORT);
        }
        mvprintw(1, 0, "Connections:\n");
        for (int i = 0; i < global_ip_num; ++i) {
            mvprintw(i + 2, 0 , "Player %d has connected\n", i + 1);
        }
        mvprintw(9, 0,  "Press enter to continue\n");
        refresh();
    }
    clear();
}

ServerUtils *startServer(Game *game) {
    int port = PORT;

    (void) signal(SIGINT, clean);

    if ((list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error creating listening socket.\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in *serverAddress = malloc(sizeof(struct sockaddr_in));
    memset(serverAddress, 0, sizeof(*serverAddress));
    serverAddress->sin_family = AF_INET;
    serverAddress->sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress->sin_port = htons(port);

    SO_REUSEADDR;
    int yes = 1;
    if (setsockopt(list_s,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    if (bind(list_s, (struct sockaddr *) serverAddress,
             sizeof(*serverAddress)) < 0) {
        fprintf(stderr, "Error calling bind() %d %s\n",errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((listen(list_s, 10)) == -1) {
        fprintf(stderr, "Error Listening\n");
        exit(EXIT_FAILURE);
    }

    int *addrSize = malloc(sizeof(int));
    *addrSize = sizeof(*serverAddress);

    ServerUtils *u = malloc(sizeof(ServerUtils));
    u->game = game;
    u->serverAddress = serverAddress;
    u->addrSize = addrSize;
    return u;
}
