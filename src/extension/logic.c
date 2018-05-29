#include "logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <ncurses.h>
#include <unistd.h>

#ifndef GRID_SIZE
#define GRID_SIZE 10
#endif

enum OCCUPIER {
    nothing,
    snake,
    snake_body,
    dead_snake //:'(
};

struct coordinate {
    int x;
    int y;
};

struct cell {
    enum OCCUPIER occupier;
    Coordinate coordinate;
};

struct snake {
    Cell *head;
    Cell **body;
    int length;
    int direction;
};

struct game {
    Cell **grid;
    Snake *snakes[4];
    int noOfSnakes;
    int width;
    int height;
    int tWidth;
    int tHeight;
    bool finished;
};

void printGame(Game *pGame);

bool finished(Game *pGame);

void updateGame(Game *game) ;

void updateDir(int ch, Game *pGame);

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
}

void initialiseRandomSeed(void) {
    srand(time(NULL));
}

void addSnake(Game *game) {
    int x = 0;
    int y = 0;
    do {
        x = rand() % (game->width + 1);
        y = rand() % (game->height + 1);
    } while (game->grid[y][x].occupier == snake);
    Snake *newSnake = malloc(sizeof(Snake));
    game->snakes[game->noOfSnakes] = newSnake;
    newSnake->body = malloc(game->width * game->height * sizeof(Cell *));
    newSnake->head = &game->grid[y][x];
    newSnake->head->occupier = snake;
    newSnake->length = 0;
    newSnake->direction = 0;
    game->noOfSnakes++;
}

void printGame(Game *game) {
    clear();
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
                    c = '*';
                    break;
                case 2:
                    c = 'o';
                    break;
                case 3:
                    c = 'x';
                    break;
                default:
                    c = ' ';
            }
            printw("%c", c);
        }
        printw("#");
        if (game->width < (game->tWidth-2)) {
            printw("\n");
        }
    }
    for (int i = 0; i < game->width + 2; ++i) {
        printw("#");
    }
    if (game->height < (game->tHeight-2)) {
        printw("\n");
    }
    refresh();
}

int main(int argc, char* argv[]) {
    Game *game = malloc(sizeof(Game));
    initialiseRandomSeed();
    //Ncurses initialisation
    initscr();
    raw();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, game->tHeight, game->tWidth);
    if (argc != 3 && argc != 1) {
        printf("Call game with height and width arguments (Includes borders).\n");
        printf("Use $LINES and $COLUMNS to find them\n");
        printf("No arguments will use default max terminal size\n");
        endwin();
        exit(EXIT_FAILURE);
    } else if (argc == 3){
        game->width = (int) (strtol(argv[2], NULL, 10) - 2);
        game->height = (int) (strtol(argv[1], NULL , 10) - 2);
        if (game->width < 1 || game->height < 1) {
            printf("You can't have a game that small!\n");
            endwin();
            exit(EXIT_FAILURE);
        }
        if (game->width > (game->tWidth-2) || game->height > (game->tHeight-2)) {
            printf("You can't have a game that big!\n");
            endwin();
            exit(EXIT_FAILURE);
        }
    } else {
        game->height = game->tHeight -2;
        game->width = game->tWidth - 2;
    }
    game->grid = calloc(game->height, sizeof(Cell *));
    if (game->grid == NULL) {
        printf("Allocation failure!\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < game->height; ++i) {
        game->grid[i] = calloc(game->width, sizeof(Cell));
    }
    buildGrid(game);
    addSnake(game);
    addSnake(game);
    int ch;
    while(!game->finished) {
        printGame(game);
        usleep(150000);
        ch = getch();
        if (ch == 'x') {
            game->finished = true;
        }
        while (ch != ERR) {
            updateDir(ch, game);
            ch = getch();
        }
        updateGame(game);
    }
    endwin();
    return 0;
}

void updateDir(int ch, Game *pGame) {
    Snake *first = pGame->snakes[0];
    Snake *second = pGame->snakes[1];
    if (ch == KEY_UP) {
        first->direction = 0;
    }
    if (ch == KEY_RIGHT) {
        first->direction = 1;
    }
    if (ch == KEY_DOWN) {
        first->direction = 2;
    }
    if (ch == KEY_LEFT) {
        first->direction = 3;
    }
    if (ch == 'w') {
        second->direction = 0;
    }
    if (ch == 'd') {
        second->direction = 1;
    }
    if (ch == 's') {
        second->direction = 2;
    }
    if (ch == 'a') {
        second->direction = 3;
    }
}

int getXOffset(Snake *snake) {
    switch (snake->direction) {
        case 0:
            return  0;
        case 1:
            return 1;
        case 2:
            return 0;
        case 3:
            return -1;
        default:
            return 0;
    }
}

int getYOffset(Snake *snake) {
    switch (snake->direction) {
        case 0:
            return -1;
        case 1:
            return 0;
        case 2:
            return 1;
        case 3:
            return 0;
        default:
            return 0;
    }
}

void updateSnake(Game *game, Snake *theSnake) {
    int xOffset = getXOffset(theSnake);
    int yOffset = getYOffset(theSnake);
    int nextX = (theSnake->head->coordinate.x + xOffset) % game->width;
    int nextY = (theSnake->head->coordinate.y + yOffset) % game->height;
    if (nextX < 0) {
        nextX = game->width - 1;
    }
    if (nextY < 0) {
        nextY = game->height - 1;
    }
    Cell *next = &game->grid[nextY][nextX];
    theSnake->head->occupier = nothing;
    theSnake->head = next;
    theSnake->head->occupier = snake;
}

void updateGame(Game *game) {
    for (int i = 0; i < game->noOfSnakes; ++i) {
        updateSnake(game, game->snakes[i]);
    }
}

