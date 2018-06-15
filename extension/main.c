#include <stdlib.h>
#include <pthread.h>
#include <zconf.h>
#include <sys/time.h>
#include "logic.h"
#include "config.h"
#include "menus.h"
#include "structs.h"
#include "server.h"

int list_s;
char **global_ips;
int global_ip_num = 0;

int main(int argc, char* argv[]) {
    Game *game = calloc(1, sizeof(Game));
    initialiseRandomSeed();
    initialiseCurses();
    initialiseColors();

    getTerminalSize(&game->tHeight, &game->tWidth);
    //Get the terminal height and width

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
    } else {
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

    setGameVariables(game);
    //Assigns correct coordinates to grid
    buildGrid(game);

    game->players = selectFromMenu();
    bool server = false;
    ServerUtils *u;
    pthread_t thread_id;
    global_ips = calloc(MAX_PLAYERS, sizeof(double *));

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

    loadNN(game);

    addSnakes(game);

    int ch;
    addFoods(game, game->foodAmount);
    printGame(game);
    //Allow 3 seconds for players to see what is happening
    usleep(300000);
    struct timeval start, next;
    gettimeofday(&start, 0);
    float elapsed = 0;
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