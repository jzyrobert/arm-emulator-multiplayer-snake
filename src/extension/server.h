#include "structs.h"
#ifndef ARM11_01_SERVER_H
#define ARM11_01_SERVER_H
Request parseRequest(int sock);

void writeFile(Request request, int soc);

int getIP(const char *ip, Game *game);

void returnFile(Request request, int con, Game *game);

void clean(int sig);

void processCommand(Game *game,Request request);

void *processPosts(void *ptr);

ServerUtils *startServer(Game *game);

void waitForConnections(ServerUtils *u);
#endif //ARM11_01_SERVER_H
