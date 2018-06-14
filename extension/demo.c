#include <stdlib.h>
#include <pthread.h>
#include <zconf.h>
#include <sys/time.h>
#include "logic.h"
#include "democonfig.h"
#include "menus.h"
#include "structs.h"

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

    if (game->tWidth < 100 || game->tHeight < 30) {
        endwin();
        printf("You can't have a game that small!\n");
        exit(EXIT_FAILURE);
    } else {
        //-2 because of the border
        game->height = game->tHeight - 2;
        game->width = game->tWidth - 2;
    }

    setGameVariables(game);
    //Assigns correct coordinates to grid
    buildGrid(game);

    game->players = 0;
    game->noOfBots = 7;

    game->AI = annMenu();

    loadNN(game);

    addSnakes(game);

    int ch;
    addFoods(game, game->foodAmount);
    printGame(game);
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
    endgame(game);
    endwin();
    freeEverything(game);
    return 0;
}