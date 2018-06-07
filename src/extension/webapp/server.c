#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <memory.h>
#include <zconf.h>
#include <netinet/in.h>
#include <bits/signum.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>

int list_s; //Listening socket

typedef struct {
    char* dir;
    char *msg;
    char *file;
    bool get;
    char* ip;
}Request;

char *globa_ips[7];
int global_ip_num = 0;

void parseBody(Request *request, char buff[]) {
    //TODO: parse body to extract arguments and set them in the request strcut
}

Request parseRequest(int sock) {
    FILE *fileStream;
    fileStream = fdopen(sock, "r");
    Request request;
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
                parseBody(&request, buff);
                char* temp = malloc(sizeof(char) * (1 + strlen(buff)));
                strcpy(temp, buff);
                request.dir = temp;
            }
            break;
        }
        if(strstr(buff, "GET") != NULL){
            sscanf(buff, "GET %s HTTP/1.1", file);
            request.get = true;
        }
        if(strstr(buff, "POST") != NULL){
            sscanf(buff, "POST %s HTTP/1.1", file);
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


void writeFile(char *file, int soc){
    char *str = "HTTP/1.0 200 OK\nServer: CS241Serv v0.1\nContent-Type: text/html\n\n";
    FILE *fp;
    fp = fopen(file, "r");
    if(fp == NULL){
        puts("Failed to open file");
    }
    char buff[2000];
    write(soc ,str, strlen(str));
    fread(buff, 1, 2000, fp);
    write(soc, buff, strlen(buff));
    while(!feof(fp)){
        fread(buff, 1, 2000, fp);
        //puts(buff);
        if(strcmp(buff, "\n\r")){
            break;
        }
        write(soc, buff, strlen(buff));
    }
    write(soc, "\n", 1);
}

void returnFile(Request request, int con){
    if(!strcmp(request.file, "/")){
        bool exists = false;
        int n = 0;
        for (int i = 0; i < global_ip_num; ++i) {
            if (!strcmp(globa_ips[i], request.ip)) {
              exists = true;
              n = i;
            }
        }
        if (!exists) {
            char* ip = malloc(sizeof(char) * (strlen(request.ip)+1));
            strcpy(ip, request.ip);
            globa_ips[global_ip_num] = ip;
            n = global_ip_num;
            global_ip_num++;
        }
        char *site = malloc(sizeof(char) * 13);
        strcpy(site, "webapp");
        char nr[2];
        nr[1] = '\0';
        nr[0] = (char) (n + '0');
        strcat(site, nr);
        strcat(site, ".html");
        writeFile(site, con);
        free(site);
        return;
    }
    /*
    if(!strcmp(request.file, "/processInput.php")){
        writeFile("webapp.html", con);
        return;
    }
     */
}

void clean(int sig) {
    puts("\nCleaning");
    close(list_s);
    exit(EXIT_SUCCESS);
}

void processCommand(Request request) {
    if (request.dir) {
        char *dir = strstr(request.dir, "=");
        dir++;
        for (int i = 0; i < global_ip_num; ++i) {
            if (!strcmp(globa_ips[i], request.ip)) {
                printf("Player %d has given command %s\n", i, dir);
            }
        }
    }
    free(request.dir);
    free(request.file);
    free(request.msg);
}

int main() {
    printf("Starting server\n");
    int port = 2035;

    (void) signal(SIGINT, clean);

    if ((list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error creating listening socket.\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);

    SO_REUSEADDR;
    int yes = 1;
    if (setsockopt(list_s,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    if (bind(list_s, (struct sockaddr *) &serverAddress,
             sizeof(serverAddress)) < 0) {
        fprintf(stderr, "Error calling bind()\n");
        exit(EXIT_FAILURE);
    }

    if ((listen(list_s, 10)) == -1) {
        fprintf(stderr, "Error Listening\n");
        exit(EXIT_FAILURE);
    }

    int addrSize = sizeof(serverAddress);
    while (1) {
        int conn_s = accept(list_s, (struct sockaddr *) &serverAddress,
                            &addrSize);
        if (conn_s == -1) {
            puts("Error handling connection");
        }
        Request request = parseRequest(conn_s);
        request.ip = inet_ntoa(serverAddress.sin_addr);
        returnFile(request ,conn_s);
        processCommand(request);
        close(conn_s);
    }
    return 0;
}


