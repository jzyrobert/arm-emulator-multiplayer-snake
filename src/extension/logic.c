#include "logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <menu.h>
#include "fannF/src/include/fann.h"
#include <math.h>
#include <bits/signum.h>
#include <signal.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>

#ifndef GRID_SIZE
#define GRID_SIZE 10
#endif

#define MAX_PLAYERS 7
#define STARTING_LENGTH 4
#define COLOR_ORANGE 8
#define CHECK_LENGTH 200
#define FOOD_PERCENTAGE 10
#define GAME_REFRESH_SPEED 100

enum Occupier {
    nothing,
    head_up,
    head_down,
    head_left,
    head_right,
    snake_body,
    //dead_snake used if wishing to make dead snakes stay as obstacles
            dead_snake,
    food
};

// positive y values go down the screen
struct direction {
    int xOffset;
    int yOffset;
    int dir;
    enum Occupier headOccupier;
};

Direction upDir = {0,-1,0,head_up};
Direction rightDir = {1,0,1,head_right};
Direction downDir = {0,1,2,head_down};
Direction leftDir = {-1,0,3,head_left};
// we use head_up as the default direction Occupier
Direction noDir = {0,0,0,head_up};

struct coordinate {
    int x;
    int y;
};

struct cell {
    enum Occupier occupier;
    Coordinate coordinate;
};

struct snake {
    Cell *head;
    Cell *nextCell;
    Cell **body;
    int length;
    Direction nextDir;
    Direction direction;
    int up;
    int down;
    int left;
    int right;
    bool alive;
    bool toDie;
};

struct game {
    Cell **grid;
    Snake *snakes[MAX_PLAYERS];
    struct fann *ANN;
    int noOfSnakes;
    int noOfBots;
    int width;
    int height;
    int tWidth;
    int tHeight;
    int food;
    int foodAmount;
    int players;
    bool finished;
    bool AI;
    bool started;
};

typedef struct request Request;
int list_s;
char *globa_ips[7];
int global_ip_num = 0;

struct request{
    char* dir;
    char *msg;
    char *file;
    bool get;
    char* ip;
};

typedef struct ServerUtils {
  Game* game;
  struct sockaddr_in *serverAddress;
  int* addrSize;
} utils;

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

int getIP(const char *ip) {
    //gets index of IP or adds it if it doesn't exist yet
    for (int i = 0; i < global_ip_num; ++i) {
        if (!strcmp(globa_ips[i], ip)) {
            return i + 1;
        }
    }
    //IP doesn't exit yet
        char* newip = malloc(sizeof(char) * (strlen(ip)+1));
        strcpy(newip, ip);
        globa_ips[global_ip_num] = newip;
        global_ip_num++;
        return global_ip_num;
}

void returnFile(Request request, int con){
    if(!strcmp(request.file, "/")){
        int n = getIP(request.ip);
        char *site = malloc(sizeof(char) * 13);
        strcpy(site, "webapp/webapp");
        char nr[2];
        nr[1] = '\0';
        nr[0] = (char) (n + '0');
        strcat(site, nr);
        strcat(site, ".html");
        writeFile(site, con);
        free(site);
        return;
    }
}

void clean(int sig) {
    puts("\nCleaning");
    close(list_s);
    exit(EXIT_SUCCESS);
}

void buildGrid(Game *game) {
    for (int i = 0; i < game->height; i++) {
        for (int j = 0; j < game->width; j++) {
            game->grid[i][j].occupier = nothing;
            game->grid[i][j].coordinate.x = j;
            game->grid[i][j].coordinate.y = i;
        }
    }
    game->noOfSnakes = 0;
    game->finished = false;
    game->started = false;
}

//Generates random seed
void initialiseRandomSeed(void) {
    srand(time(NULL));
}

void addSnake(Game *game,int up, int down, int left, int right) {
    int x = 0;
    int y = 0;
    getmaxyx(stdscr, y, x);
    Snake *newSnake = malloc(sizeof(Snake));
    game->snakes[game->noOfSnakes] = newSnake;
    newSnake->body = malloc(game->width * game->height * sizeof(Cell *));
    //We place every snake evenly apart on the game screen at the start
    newSnake->head = &game->grid[y - 5][x/(game->players + game->noOfBots + 1) * (game->players + game->noOfBots - game->noOfSnakes)];
    newSnake->head->occupier = head_up;
    newSnake->length = 0;
    newSnake->direction = upDir;
    newSnake->nextDir = upDir;
    game->noOfSnakes++;
    newSnake->up = up;
    newSnake->down = down;
    newSnake->left = left;
    newSnake->right = right;
    newSnake->alive = true;
    newSnake->toDie = false;
}

//Prints the correspondingly direction and colour head character for a snake
void findHead(Game *pGame, int j, int i) {
    for (int k = 0; k < pGame->noOfSnakes; ++k) {
        if (pGame->snakes[k]->head == &pGame->grid[j][i] && pGame->snakes[k]->alive) {
            attron(COLOR_PAIR((2 + pGame->noOfSnakes) - k));
            break;
        }
    }
}

//Prints corresponding colour body character for a snake
void findBody(Game *pGame, int j, int i) {
    for (int k = 0; k < pGame->noOfSnakes; ++k) {
        for (int l = 0; l < pGame->snakes[k]->length; ++l) {
            if (pGame->snakes[k]->body[l] == &pGame->grid[j][i] && pGame->snakes[k]->alive) {
                attron(COLOR_PAIR((2 + pGame->noOfSnakes) - k));
                break;
            }
        }
    }
}

void printGame(Game *game) {
    //Each refresh we print from the top left again
    move(0,0);
    attron(COLOR_PAIR(1));
    for (int i = 0; i < game->width + 2; ++i) {
        printw("#");
    }
    if (game->width < (game->tWidth-2)) {
        printw("\n");
    }
    for (int j = 0; j < game->height; ++j) {
        printw("#");
        for (int i = 0; i < game->width; ++i) {
            char c;
            switch ((int) game->grid[j][i].occupier) {
                case 0:
                    c = ' ';
                    break;
                case 1:
                    findHead(game, j, i);
                    c = '^';
                    break;
                case 2:
                    findHead(game, j, i);
                    c = 'v';
                    break;
                case 3:
                    findHead(game, j, i);
                    c = '<';
                    break;
                case 4:
                    findHead(game, j, i);
                    c = '>';
                    break;
                case 5:
                    findBody(game, j, i);
                    c = 'o';
                    break;
                case 6:
                    c = 'x';
                    break;
                case 7:
                    attron(COLOR_PAIR(2));
                    c = '*';
                    break;
                default:
                    c = ' ';
            }
            printw("%c", c);
            attron(COLOR_PAIR(1));
        }
        printw("#");
        if (game->width < (game->tWidth-2)) {
            printw("\n");
        }
        attron(COLOR_PAIR(1));
    }
    for (int i = 0; i < game->width + 2; ++i) {
        printw("#");
    }
    if (game->height < (game->tHeight-2)) {
        printw("\n");
    }
    refresh();
}

//Allows adding of num foods
void addFoods(Game *game, int num) {
    for (int i = 0; i < num; ++i) {
        addFood(game);
    }
}

//calculates time difference in milliseconds between t0 and t1
float timedifference_msec(struct timeval t0, struct timeval t1)
{
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

//Frees the grid, each snake and then the struct game
void freeEverything(Game *pGame) {
    for (int i = 0; i < pGame->height; ++i) {
        free(pGame->grid[i]);
    }
    free(pGame->grid);
    for (int j = 0; j < pGame->noOfSnakes; ++j) {
        free(pGame->snakes[j]->body);
        free(pGame->snakes[j]);
    }
    free(pGame);
}

//Chooses whether to use AI for moves
bool annMenu(void) {
    char *choices[] = {
            "Bots use Rob's dummy AI to move",
            "Bots use a trained Neural Network to move"
    };
    ITEM **bot_num;
    MENU *bot_menu;
    ITEM *cur_item = NULL;

    int c;
    int Num_choices = 2;
    bot_num = calloc(Num_choices + 1, sizeof(ITEM *));
    for (int k = 0; k < Num_choices; ++k) {
        bot_num[k] = new_item(choices[k], "");
    }
    bot_num[Num_choices] = (ITEM *) NULL;

    bot_menu = new_menu(bot_num);
    mvprintw(LINES - 2, 2, "X to Exit");
    post_menu(bot_menu);
    refresh();
    while ((c = getch()) != 'x') {
        switch (c) {
            case KEY_DOWN:
                menu_driver(bot_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(bot_menu, REQ_UP_ITEM);
                break;
            case 10:
                cur_item = current_item(bot_menu);
                break;
        }
        //For some reason KEY_ENTER isn't recognised but 10 is the actual enter key ID
        if (c == KEY_ENTER || c == 10) {
            break;
        }
    }
    if (c == 'x') {
        endwin();
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < Num_choices; ++i) {
        if (cur_item == bot_num[i]) {
            unpost_menu(bot_menu);
            for (int l = 0; l < Num_choices + 1; ++l) {
                free_item(bot_num[l]);
            }
            free_menu(bot_menu);
            free(bot_num);
            return (bool) i;
        }
    }
    return 0;
}

//Allows choosing number of bots based on number of players
int botnumMenu(Game *game) {
    char *choices[] = {
            "1 bot",
            "2 bots",
            "3 bots",
            "4 bots",
            "5 bots",
            "6 bots",
    };

    ITEM **bot_num;
    MENU *bot_menu;
    ITEM *cur_item = NULL;
    int c;
    int Num_choices = MAX_PLAYERS - game->players;
    bot_num = calloc(Num_choices + 1, sizeof(ITEM *));
    for (int k = 0; k < Num_choices; ++k) {
        bot_num[k] = new_item(choices[k], "");
    }
    bot_num[Num_choices] = (ITEM *) NULL;

    bot_menu = new_menu(bot_num);
    mvprintw(LINES - 2, 2, "X to Exit");
    post_menu(bot_menu);
    refresh();
    while ((c = getch()) != 'x') {
        switch (c) {
            case KEY_DOWN:
                menu_driver(bot_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(bot_menu, REQ_UP_ITEM);
                break;
            case 10:
                cur_item = current_item(bot_menu);
                break;
        }
        if (c == KEY_ENTER || c == 10) {
            break;
        }
    }
    if (c == 'x') {
        endwin();
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < Num_choices; ++i) {
        if (cur_item == bot_num[i]) {
            unpost_menu(bot_menu);
            for (int l = 0; l < Num_choices + 1; ++l) {
                free_item(bot_num[l]);
            }
            free_menu(bot_menu);
            free(bot_num);
            return i + 1;
        }
    }
    return 0;
}

//To include bots or not
bool botMenu(void) {
    char *choices[] = {
            "Don't include bots",
            "Include bots"
    };
    ITEM **bot_num;
    MENU *bot_menu;
    ITEM *cur_item = NULL;
    int c;
    int Num_choices = 2;
    bot_num = calloc(Num_choices + 1, sizeof(ITEM *));
    for (int k = 0; k < Num_choices; ++k) {
        bot_num[k] = new_item(choices[k], "");
    }
    bot_num[Num_choices] = (ITEM *) NULL;

    bot_menu = new_menu(bot_num);
    mvprintw(LINES - 2, 2, "X to Exit");
    post_menu(bot_menu);
    refresh();
    while ((c = getch()) != 'x') {
        switch (c) {
            case KEY_DOWN:
                menu_driver(bot_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(bot_menu, REQ_UP_ITEM);
                break;
            case 10:
                cur_item = current_item(bot_menu);
                break;
        }
        if (c == KEY_ENTER || c == 10) {
            break;
        }
    }
    if (c == 'x') {
        endwin();
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < Num_choices; ++i) {
        if (cur_item == bot_num[i]) {
            unpost_menu(bot_menu);
            for (int l = 0; l < Num_choices + 1; ++l) {
                free_item(bot_num[l]);
            }
            free_menu(bot_menu);
            free(bot_num);
            return (bool) i;
        }
    }
    return 0;
}

//Title screen menu
int selectFromMenu(void) {
    char *choices[] = {
            "1 Player (Arrow keys) - Green",
            "2 Players (WASD) - Blue",
            "3 Players (TFGH) - Cyan",
            "4 Players (IJKL) - Magenta",
            "5 Players ([;'#) - Red",
            "6 Players (5123) - Orangey Brown",
            "7 Players (Home/Del/End/PgDown) - Yellow",
    };
    ITEM **player_num;
    MENU *player_menu;
    ITEM *cur_item = NULL;
    int c;
    int Num_choices = MAX_PLAYERS;
    player_num = calloc(Num_choices + 1, sizeof(ITEM *));
    for (int k = 0; k < Num_choices; ++k) {
        player_num[k] = new_item(choices[k], "");
    }
    player_num[Num_choices] = (ITEM *)NULL;

    player_menu = new_menu(player_num);
    char *title[] = {
            "         _______..__   __.      ___       __  ___  _______    ____    __    ____  ___      .______          _______.",
            "        /       ||  \\ |  |     /   \\     |  |/  / |   ____|   \\   \\  /  \\  /   / /   \\     |   _  \\        /       |",
            "       |   (----`|   \\|  |    /  ^  \\    |  '  /  |  |__       \\   \\/    \\/   / /  ^  \\    |  |_)  |      |   (----`",
            "        \\   \\    |  . `  |   /  /_\\  \\   |    <   |   __|       \\            / /  /_\\  \\   |      /        \\   \\    ",
            "    .----)   |   |  |\\   |  /  _____  \\  |  .  \\  |  |____       \\    /\\    / /  _____  \\  |  |\\  \\----.----)   |",
            "    |_______/    |__| \\__| /__/     \\__\\ |__|\\__\\ |_______|       \\__/  \\__/ /__/     \\__\\ | _| `._____|_______/"
    };
    for (int j = 0; j < 6; ++j) {
        mvprintw(LINES/2 + j,0,title[j]);
    }
    mvprintw(LINES - 2, 2, "X to Exit");
    post_menu(player_menu);
    refresh();
    while ((c = getch()) != 'x') {
        switch(c) {
            case KEY_DOWN:
                menu_driver(player_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(player_menu, REQ_UP_ITEM);
                break;
            case 10:
                cur_item = current_item(player_menu);
                break;
        }
        if (c == KEY_ENTER || c == 10) {
            break;
        }
    }
    if (c == 'x') {
        endwin();
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < Num_choices; ++i) {
        if (cur_item == player_num[i]) {
            unpost_menu(player_menu);
            for (int l = 0; l < Num_choices + 1; ++l) {
                free_item(player_num[l]);
            }
            free_menu(player_menu);
            free(player_num);
            return i + 1;
        }
    }
    unpost_menu(player_menu);
    for (int l = 0; l < Num_choices + 1; ++l) {
        free_item(player_num[l]);
    }
    free_menu(player_menu);
    free(player_num);
    return 0;
}

//If the exit button (X) is pressed during player selection
void printNoPlayers() {
    int x;
    int y;
    getmaxyx(stdscr, y, x);
    clear();
    char msg1[] = "No players selected";
    char msg2[] = "Press any button to exit";
    mvprintw(y/2, x/2 - strlen(msg1) / 2, "%s",msg1);
    mvprintw(y/2 + 1, x/2 - strlen(msg2) / 2, "%s",msg2);
    refresh();
    nodelay(stdscr, false);
    getch();
}

void processCommand(Game *game,Request request) {
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
            if (!strcmp(globa_ips[i], request.ip)) {
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
    utils *u = (utils *) ptr;
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

bool servermenu() {
        char *choices[] = {
                "Play locally",
                "Start server and enable LAN connections"
        };
        ITEM **server_num;
        MENU *server_menu;
        ITEM *cur_item = NULL;
        int c;
        int Num_choices = 2;
        server_num = calloc(Num_choices + 1, sizeof(ITEM *));
        for (int k = 0; k < Num_choices; ++k) {
            server_num[k] = new_item(choices[k], "");
        }
        server_num[Num_choices] = (ITEM *) NULL;

        server_menu = new_menu(server_num);
        mvprintw(LINES - 2, 2, "X to Exit");
        post_menu(server_menu);
        refresh();
        while ((c = getch()) != 'x') {
            switch (c) {
                case KEY_DOWN:
                    menu_driver(server_menu, REQ_DOWN_ITEM);
                    break;
                case KEY_UP:
                    menu_driver(server_menu, REQ_UP_ITEM);
                    break;
                case 10:
                    cur_item = current_item(server_menu);
                    break;
            }
            if (c == KEY_ENTER || c == 10) {
                break;
            }
        }
        if (c == 'x') {
            endwin();
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < Num_choices; ++i) {
            if (cur_item == server_num[i]) {
                unpost_menu(server_menu);
                for (int l = 0; l < Num_choices + 1; ++l) {
                    free_item(server_num[l]);
                }
                free_menu(server_menu);
                free(server_num);
                return (bool) i;
            }
        }
        return 0;
    }

utils *startServer(Game *game) {
    int port = 2035;

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

    utils *u = malloc(sizeof(utils));
    u->game = game;
    u->serverAddress = serverAddress;
    u->addrSize = addrSize;
    return u;
}

void waitForConnections(utils *u) {
    clear();
    int ch;
    while((ch = getch()) != 10) {
        if (ch == 'x') {
            endwin();
            exit(EXIT_FAILURE);
        }
        move(0,0);
        printw("Connections:\n");
        for (int i = 0; i < global_ip_num; ++i) {
            mvprintw(i + 1, 0 , "Player %d has connected\n", i + 1);
        }
        mvprintw(8, 0,  "Press enter to continue\n");
        refresh();
    }
    clear();
}

int main(int argc, char* argv[]) {
    Game *game = malloc(sizeof(Game));
    game->food = 0;
    initialiseRandomSeed();
    //Ncurses initialisation
    if (initscr() == NULL) {
        printf("Window failure!");
        free(game);
        exit(EXIT_FAILURE);
    }
    //Allows usage of all keyboard keys
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    //Allows input without using enter, and times out after 1 ms
    noecho();
    cbreak();
    timeout(1);

    start_color();
    int background = COLOR_BLACK;
    //Assiging custom RGB colour to orange
    init_color(8, 204,120, 50);

    init_pair(1, COLOR_WHITE, background);
    init_pair(2, COLOR_WHITE, background);
    init_pair(3, COLOR_GREEN, background);
    init_pair(4, COLOR_BLUE, background);
    init_pair(5, COLOR_CYAN, background);
    init_pair(6, COLOR_MAGENTA, background);
    init_pair(7, COLOR_RED, background);
    init_pair(8, COLOR_ORANGE, background);
    init_pair(9, COLOR_YELLOW, background);

    //Get the terminal height and width
    getmaxyx(stdscr, game->tHeight, game->tWidth);

    if (argc != 3 && argc != 1) {
        endwin();
        printf("Call game with height and width arguments (Includes borders).\n");
        printf("Use $LINES and $COLUMNS to find them\n");
        printf("No arguments will use default max terminal size\n");
        exit(EXIT_FAILURE);
    } else if (argc == 3){
        game->width = (int) (strtol(argv[2], NULL, 10) - 2);
        game->height = (int) (strtol(argv[1], NULL , 10) - 2);
        if (game->width < 98 || game->height < 30) {
            endwin();
            printf("You can't have a game that small!\n");
            exit(EXIT_FAILURE);
        }
        if (game->width > (game->tWidth-2) || game->height > (game->tHeight-2)) {
            endwin();
            printf("You can't have a game that big!\n");
            exit(EXIT_FAILURE);
        }
    } else if (argc == 1) {
        if (game->tWidth < 100 || game->tHeight < 30) {
            endwin();
            printf("You can't have a game that small!\n");
            exit(EXIT_FAILURE);
        } else {
            //-2 because of the border
            game->height = game->tHeight - 2;
            game->width = game->tWidth - 2;
        }
    }

    //% of screen to be covered in food
    game->foodAmount = (game->height * game->width) / FOOD_PERCENTAGE;
    game->grid = calloc(game->height, sizeof(Cell *));
    if (game->grid == NULL) {
        printf("Allocation failure!\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < game->height; ++i) {
        game->grid[i] = calloc(game->width, sizeof(Cell));
    }
    //Assigns correct coordinates to grid
    buildGrid(game);

    game->players = selectFromMenu();
    bool server = false;
    utils *u;
    pthread_t thread_id;

    if (game->players > 0) {
        server = servermenu();
        if (server) {
            u = startServer(game);
            pthread_create(&thread_id, NULL, processPosts, (void *) u);
            waitForConnections(u);
        }
        if (botMenu()) {
            game->noOfBots = botnumMenu(game);
            //Whether to use the neural net or not
            game->AI = annMenu();
        } else {
            game->noOfBots = 0;
        }
    } else {
        endwin();
        exit(EXIT_FAILURE);
    }

    //loads neural net
    if (game->AI) {
        game->ANN = fann_create_from_file("ANN.net");
    }

    //Snakes are added in reverse order for fallthrough convenience. snake 0 is always the last snake.
    switch (game->players + game->noOfBots) {
        case 7:
            addSnake(game, KEY_HOME, KEY_END, KEY_DC, KEY_NPAGE);
        case 6:
            addSnake(game, '5', '2', '1', '3');
        case 5:
            addSnake(game, '[', '\'', ';', '#');
        case 4:
            addSnake(game, 'i', 'k', 'j', 'l');
        case 3:
            addSnake(game, 't', 'g', 'f', 'h');
        case 2:
            addSnake(game, 'w', 's', 'a', 'd');
        case 1:
            addSnake(game, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT);
            break;
        default:
            printNoPlayers();
            endwin();
            freeEverything(game);
            exit(EXIT_FAILURE);
    }
    //Make every snake STARTING LENGTH longer.
    int x = STARTING_LENGTH;
    for (int j = 0; j < game->noOfSnakes; ++j) {
        for (int i = 0; i < x; ++i) {
            addLength(game, game->snakes[j]);
        }
    }

    int ch;
    printGame(game);
    //Allow 3 seconds for players to see what is happening
    usleep(100000);
    struct timeval start, next;
    gettimeofday(&start, 0);
    float elapsed = 0;
    clear();
    game->started = true;
    while (!game->finished) {
        gettimeofday(&next, 0);
        elapsed = timedifference_msec(start, next);
        //Screen refreshes every SPEED milliseconds
                if (elapsed > GAME_REFRESH_SPEED) {
                gettimeofday(&start, 0);
                updateGame(game);
                printGame(game);
            }

            //Constantly loops for input
            if ((ch = getch()) != ERR) {
                updateDir(ch, game);
            }
        }
    if (server) {
         pthread_cancel(thread_id);
         free(u->addrSize);
         free(u->serverAddress);
         free(u);
    }
    endgame(game);
    endwin();
    freeEverything(game);
    return 0;
}

void endgame(Game *game) {
    int x;
    int y;
    int ch;
    int l = STARTING_LENGTH;
    getmaxyx(stdscr, y, x);
    clear();
    char msg1[] = "The game has ended!";
    char msg3[] = "Scores:";
    char msg2[] = "Press X to exit";
    mvprintw(y/2 - 3, x/2 - strlen(msg1) / 2, "%s",msg1);
    mvprintw(y/2 - 2, x/2 - strlen(msg3) / 2, "%s",msg3);
    for (int i = game->noOfSnakes - 1; i >= 0; --i) {
        // Print out the snake's scores in the middle of the screen
        attron(COLOR_PAIR((2 + game->noOfSnakes) - i));
        mvprintw(y/2 - 2 + (game->noOfSnakes - i), x/2 - strlen(msg3) / 2, "Snake %d: %d", game->noOfSnakes-i, game->snakes[i]->length - l);
    }
    attron(COLOR_PAIR(1));
    mvprintw(y/2 - 1 + game->noOfSnakes, x/2 - strlen(msg2) / 2, "%s",msg2);
    refresh();
    nodelay(stdscr, false);
    //Loop to not exit until x is pressed
    while ((ch = getch()) != 'x') {

    }
}

//Collects all empty cells and assigns food to a random location
void addFood(Game *pGame) {
    Cell *empty[pGame->width * pGame->height];
    int n = 0;
    for (int i = 0; i < pGame->height; ++i) {
        for (int j = 0; j < pGame->width; ++j) {
            if (pGame->grid[i][j].occupier == nothing) {
                empty[n] = &pGame->grid[i][j];
                n++;
            }
        }
    }
    if (n > 0) {
        pGame->food++;
        int x = rand() % n;
        empty[x]->occupier = food;
    }
}

//Takes the input and changes the corresponding snakes direction
void updateDir(int ch, Game *pGame) {
    if (ch == 'x') {
        pGame->finished = true;
    } else {
        for (int i = 0; i < pGame->noOfSnakes; ++i) {
            Direction direction = noDir;
            if (ch == pGame->snakes[i]->up) {
                direction = upDir;
            }
            if (ch == pGame->snakes[i]->right) {
                direction = rightDir;
            }
            if (ch == pGame->snakes[i]->down) {
                direction = downDir;
            }
            if (ch == pGame->snakes[i]->left) {
                direction = leftDir;
            }
            if ((direction.xOffset != 0) || (direction.yOffset != 0)) {
                if (!oppositeDir(pGame->snakes[i], direction)) {
                    pGame->snakes[i]->nextDir = direction;
                }
            }
        }
    }
}

//Returns true if opposite dir (up/down left/right)
bool oppositeDir(Snake *pSnake, Direction newDirection) {
    Direction oldDirection = pSnake->direction;
    return oldDirection.xOffset != 0 ? oldDirection.yOffset + newDirection.yOffset == 0 :
           oldDirection.xOffset + newDirection.xOffset == 0;
}

int getXOffset(Snake *snake) {
    return snake->nextDir.xOffset;
}

int getYOffset(Snake *snake) {
    return snake->nextDir.yOffset;
}

enum Occupier getHeadChar(Snake *theSnake) {
    return theSnake->nextDir.headOccupier;
}

//Gets the next cell in the next direction of a snakes head
Cell* getNextCell(Game *game, Snake *snake){
    int xOffset = getXOffset(snake);
    int yOffset = getYOffset(snake);
    int nextX = (snake->head->coordinate.x + xOffset) % game->width;
    int nextY = (snake->head->coordinate.y + yOffset) % game->height;
    if (nextX < 0) {
        nextX = game->width - 1;
    }
    if (nextY < 0) {
        nextY = game->height - 1;
    }
    return &game->grid[nextY][nextX];
}

//Turns a dead snake into food, optionally obstacles
void killSnake(Game *game, Snake *snake) {
    snake->alive = false;
    snake->head->occupier = food;
    //snake->head->occupier = getHeadChar(snake);
    game->food++;
    for (int i = 0; i < snake->length; ++i) {
        snake->body[i]->occupier = food;
        //snake->body[i]->occupier = dead_snake;
        game->food++;
    }
}

//Adjusts a snake during its next move
void updateSnake(Game *game, Snake *snake) {
        Cell *next = snake->nextCell;
        if (next->occupier != nothing && next->occupier != food) {
            //rip snake
            killSnake(game, snake);
        } else if (next->occupier == food){
            game->food--;
            moveSizeIncrease(game, snake, next);
        } else {
            moveSnake(game, snake, next);
        }
}

//Moves the body and head long
void moveSnake(Game *game, Snake *theSnake, Cell *next) {
    Cell *previous = theSnake->head;
    theSnake->head = next;
    theSnake->head->occupier = getHeadChar(theSnake);
    for (int i = 0; i < theSnake->length; ++i) {
        next = previous;
        previous = theSnake->body[i];
        theSnake->body[i] = next;
        theSnake->body[i]->occupier = snake_body;
    }
    theSnake->direction = theSnake->nextDir;
}

//Moves the body but keeps the last cell as new end of tail
void moveSizeIncrease(Game *game, Snake *theSnake, Cell *next) {
    Cell *previous = theSnake->head;
    theSnake->head = next;
    theSnake->head->occupier = getHeadChar(theSnake);
    for (int i = 0; i < theSnake->length; ++i) {
        next = previous;
        previous = theSnake->body[i];
        theSnake->body[i] = next;
        theSnake->body[i]->occupier = snake_body;
    }
    theSnake->body[theSnake->length] = previous;
    theSnake->length++;
    theSnake->direction = theSnake->nextDir;
}

//For convenience, gets next cell and makes snake increase size
void addLength(Game *game, Snake *theSnake) {
    Cell *next = getNextCell(game, theSnake);
    moveSizeIncrease(game, theSnake, next);
}

Direction getDirection(int choice) {
    switch (choice) {
        case 0:
            return upDir;
        case 1:
            return rightDir;
        case 2:
            return downDir;
        case 3:
            return leftDir;
    }
    return noDir;
}

//Gets cell N ahead in next direction
Cell getCellAhead(Game *game, Snake *snake, int n) {
    int x = snake->head->coordinate.x;
    int y = snake->head->coordinate.y;
    x = (x + snake->nextDir.xOffset * n) % game->width;
    y = (y + snake->nextDir.yOffset * n) % game->height;
    while (x < 0) {
        x += game->width;
    }
    while (y < 0) {
        y += game->height;
    }
    return game->grid[y][x];
}

//Rob's dummy AI, snake will choose path of least resistance starting from a distance chech of CHECK cells ahead
void calculateNextMove(Game *game, Snake *pSnake, int check) {
    while (check > 0) {
        int availableMoves = 0;
        int movesID[3];
        for (int i = 0; i < 4; ++i) {
            if (!oppositeDir(pSnake, getDirection(i))) {
                bool available = true;
                pSnake->nextDir = getDirection(i);
                for (int j = 1; j <= check; ++j) {
                    Cell toCheck = getCellAhead(game, pSnake, j);
                    if ((toCheck.occupier != food) && (toCheck.occupier != nothing)) {
                        available = false;
                    }
                }
                if (available) {
                    movesID[availableMoves] = i;
                    availableMoves++;
                }
            }
        }
        if (availableMoves > 0) {
            pSnake->nextDir = getDirection(movesID[rand() % availableMoves]);
            break;
        } else {
            check--;
        }
    }
}

//Gets next cell in a certain direction (0-3)
Cell * nextDirectionCell(Game *pGame, Cell *pCell, int i) {
    Direction dir = getDirection(i);
    int nextX = (pCell->coordinate.x + dir.xOffset + pGame->width) % pGame->width;
    int nextY = (pCell->coordinate.y + dir.yOffset + pGame->height) % pGame->height;
    return &pGame->grid[nextY][nextX];
}

//Checks whether the cell is in the array
bool notIn(Cell **pCell, Cell *next, int n) {
    for (int i = 0; i < n; ++i) {
        if (pCell[i] == next) {
            return false;
        }
    }
    return true;
}

//Breadth first search of the nearest food, ignoring obstacles. If none found (dead end) then returns the snakes head
Cell * nearestFoodCell(Game *game, Cell *start) {
    //start a breadth first search
    Cell **seen = malloc(game->width * game->height * sizeof(Cell *));
    Cell **queue = malloc(game->width * game->height * sizeof(Cell *));
    int n = 0;
    int q = 0;
    int qc = 0;
    Cell *current = start;
    while (current && (current->occupier != food)) {
        seen[n] = current;
        n++;
        for (int i = 0; i < 4; ++i) {
            Cell *next = nextDirectionCell(game, current, i);
            if (notIn(seen, next, n) && notIn(queue, next, q) && (next->occupier == nothing || next->occupier == food)) {
                queue[q] = next;
                q++;
            }
        }
        current = queue[qc];
        qc++;
    }
    free(seen);
    free(queue);
    if (current) {
        return current;
    } else {
        return start;
    }
}

//Returns -1/0/1 (eg whether move is left/forwards/right) based on new to old direction
int getMoveID(Direction old, Direction new) {
    switch(old.dir) {
        case 0:
            switch (new.dir) {
                case 1:
                    return 1;
                case 3:
                    return -1;
                default:
                    return 0;
            }
        case 1:
            switch (new.dir) {
                case 0:
                    return -1;
                case 2:
                    return 1;
                default:
                    return 0;
            }
        case 2:
            switch (new.dir) {
                case 1:
                    return -1;
                case 3:
                    return 1;
                default:
                    return 0;
            }
        case 3:
            switch (new.dir) {
                case 0:
                    return 1;
                case 2:
                    return -1;
                default:
                    return 0;
            }
    }
    return 0;
}

//Calculates normalised angle (-1 to 1) of cell food to pCell, in regards to direction of the snake
double angleBasedOnDirection(Game *game, Snake *pSnake, Cell *pCell, Cell *food) {
    double angle = 0;
    int xd, yd;
    int xd1 = food->coordinate.x - pCell->coordinate.x;
    if (pCell->coordinate.x < food->coordinate.x) {
        int xd2 = food->coordinate.x - (pCell->coordinate.x + game->width);
        xd = abs(xd1) < abs(xd2) ? xd1 : xd2;
    } else {
        int xd2 = (food->coordinate.x + game->width) - pCell->coordinate.x;
        xd = abs(xd1) < abs(xd2) ? xd1 : xd2;
    }
    int yd1 = -(food->coordinate.y - pCell->coordinate.y);
    //because y grows downwards but we want to calculate angle as if it went upwards
    if (pCell->coordinate.y < food->coordinate.y) {
        int yd2 = (pCell->coordinate.y + game->height) - food->coordinate.y;
        yd = abs(yd1) < abs(yd2) ? yd1 : yd2;
    } else {
        int yd2 = pCell->coordinate.y - (food->coordinate.y + game->height);
        yd = abs(yd1) < abs(yd2) ? yd1 : yd2;
    }
    switch (pSnake->direction.dir) {
        case 0:
            angle = atan2(xd, yd);
            break;
        case 1:
            angle = atan2(-yd, xd);
            break;
        case 2:
            angle = atan2(-xd, -yd);
            break;
        case 3:
            angle = atan2(yd, -xd);
            break;
    }
    return angle / M_PI;
}

void calcFann(Game *pGame, Snake *pSnake) {
    float max = -1;
    int c = 0;
    for (int i = -1; i < 2; ++i) {
        fann_type *calc_out;
        fann_type input[8];
        int n = 0;
        for (int j = -1; j < 2; ++j) {
            pSnake->nextDir = getDirection((pSnake->direction.dir + j + 4) % 4);
            input[n] = getNextCell(pGame, pSnake)->occupier != nothing && getNextCell(pGame, pSnake)->occupier != food;
            n++;
        }
        for (int k = -1; k < 2; ++k) {
            pSnake->nextDir = getDirection((pSnake->direction.dir + k + 4) % 4);
            input[n] = getNextCell(pGame, pSnake)->occupier == food;
            n++;
        }
        pSnake->nextDir = getDirection((pSnake->direction.dir + i + 4) % 4);
        Cell *food = nearestFoodCell(pGame, pSnake->head);
        double angle = angleBasedOnDirection(pGame, pSnake, pSnake->head, food);
        input[6] = (fann_type) angle;
        input[7] = getMoveID(pSnake->direction, pSnake->nextDir);
        calc_out = fann_run(pGame->ANN, input);
        if (calc_out[0] > max) {
            max = calc_out[0];
            c = i;
        } else if (calc_out[0] == max) {
            if (rand() % 2) {
                c = i;
            }
        }
    }
    pSnake->nextDir = getDirection((pSnake->direction.dir + c + 4) % 4);
}

void updateGame(Game *game) {
    int dead = 0;
            //bots will try not to hit things
        for (int m = 0; m < game->noOfBots; ++m) {
            if (game->AI) {
                calcFann(game, game->snakes[m]);
            } else {
                calculateNextMove(game, game->snakes[m], CHECK_LENGTH);
            }
        }
    //check all tails and heads
    for (int j = 0; j < game->noOfSnakes; ++j) {
        if (game->snakes[j]->alive) {
            game->snakes[j]->nextCell = getNextCell(game, game->snakes[j]);
            for (int i = j - 1; i >= 0 ; --i) {
                if (game->snakes[i]->alive) {
                    //2 Snakes will die if they collide at an angle
                    if (game->snakes[j]->nextCell == game->snakes[i]->nextCell) {
                        game->snakes[j]->toDie = true;
                        game->snakes[i]->toDie = true;
                    }
                }
            }
        }
    }
    for (int k = 0; k < game->noOfSnakes; ++k) {
        if (game->snakes[k]->toDie) {
            killSnake(game, game->snakes[k]);
            game->snakes[k]->toDie = false;
        }
    }
    //check tails of remaining snakes
    for (int l = 0; l < game->noOfSnakes; ++l) {
        if (game->snakes[l]->alive) {
            if (game->snakes[l]->nextCell->occupier != food) {
                game->snakes[l]->body[game->snakes[l]->length - 1]->occupier = nothing;
            }
        }
    }
    //Adjust alive snakes
    for (int i = 0; i < game->noOfSnakes; ++i) {
        if (game->snakes[i]->alive) {
            updateSnake(game, game->snakes[i]);
        } else {
            dead++;
        }
    }
    //If more than 1 snake, we end game when there is 1 snake left to make games faster.
    if ((game->noOfSnakes > 1 && dead == (game->noOfSnakes - 1)) || (game->noOfSnakes == 1 && dead == 1)) {
        game->finished = true;
    }
    //Replenishes food back to the amount required.
    if (game->food < game->foodAmount) {
        addFoods(game, game->foodAmount - (game->food));
    }
}

