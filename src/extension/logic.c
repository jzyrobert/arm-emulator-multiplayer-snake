#include "logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <ncurses.h>

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
};

void printGame(Game *pGame);

void buildGrid(Game *game) {
    for (int i = 0; i < game->height; i++) {
        for (int j = 0; j < game->width; j++) {
            game->grid[i][j].occupier = nothing;
            game->grid[i][j].coordinate.x = j;
            game->grid[i][j].coordinate.y = i;
        }
    }
    game->noOfSnakes = 0;
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
    game->noOfSnakes++;
}

void printGame(Game *game) {
    int x;
    int y;
    getmaxyx(stdscr, y, x);
    for (int i = 0; i < game->width + 2; ++i) {
        printw("#");
    }
    if (game->width < (x-2)) {
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
        if (game->width < (x-2)) {
            printw("\n");
        }
    }
    for (int i = 0; i < game->width + 2; ++i) {
        printw("#");
    }
    if (game->width < (x-2)) {
        printw("\n");
    }
    refresh();
    getch();
}

int main(int argc, char* argv[]) {
    Game *game = malloc(sizeof(Game));
    initialiseRandomSeed();
    initscr();
    if (argc != 3 && argc != 1) {
        printf("Call game with height and width arguments.\n Use $LINES and $COLUMNS to find them\n");
        printf("No arguments will use default max terminal size");
        exit(EXIT_FAILURE);
    } else if (argc == 3){
        game->width = strtol(argv[2], NULL, 10) - 2;
        game->height = strtol(argv[1], NULL ,10) - 2;
        if (game->width < 1) {
            printf("You can't have a game that small!");
            exit(EXIT_FAILURE);
        }
        if (game->height < 1) {
            printf("You can't have a game that small!");
        }
    } else {
        getmaxyx(stdscr, game->height, game->width);
        game->height -= 2;
        game->width -= 2;
    }
    game->grid = calloc(game->height, sizeof(Cell *));
    for (int i = 0; i < game->height; ++i) {
        game->grid[i] = calloc(game->width, sizeof(Cell));
    }
    if (game->grid == NULL) {
        printf("Allocation failure!\n");
        exit(EXIT_FAILURE);
    }
    buildGrid(game);
    addSnake(game);
    printGame(game);
    endwin();
    return 0;
}

