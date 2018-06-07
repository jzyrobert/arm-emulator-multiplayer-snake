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

enum DIRECTION{
    Up,
    Down,
    Left,
    Right
};



typedef struct {
    enum DIRECTION dir;
    char *msg;
    char *file;
    bool get;
}Request;

void parseBody(Request *request, char buff[]) {
    //TODO: parse body to extract arguments and set them in the request strcut
}

Request parseRequest(int sock) {
    FILE *fileStream;
    fileStream = fdopen(sock, "r");
    Request request;
    size_t size = 1;
    char *msg = '\0';
    size = 1;
    char buff[2000];
    int count = 0;
    int lim = 0;

    char *dir = malloc(sizeof(char) * 10);
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
                puts(buff);
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
    static int n = 0;
    if(!strcmp(request.file, "/")){
        char *site = malloc(sizeof(char) * 13);
        strcpy(site, "webapp");
        char nr[2];
        nr[1] = '\0';
        nr[0] = (char) (n + '0');
        strcat(site, nr);
        strcat(site, ".html");
        writeFile(site, con);
        return;
    }
    n++;
    n = n % 2;
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

int main() {
    printf("Starting server\n");
    int port = 2034;

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
        printf("ip is: %s\n", inet_ntoa(serverAddress.sin_addr));
        Request request = parseRequest(conn_s);
        returnFile(request ,conn_s);
        close(conn_s);
    }
    return 0;
}


