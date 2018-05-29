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
    dead_snake
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
    Cell head;
    Cell body[200];
};

struct game {
    Cell **grid;
    Snake snakes[4];
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
}

void initialiseRandomSeed(void) {
    srand(time(NULL));
}

Snake addSnake(Game *game) {
    int x = 0;
    int y = 0;
    do {
        x = rand() % (game->width + 1);
        y = rand() % (game->height + 1);
    } while (game->grid[y][x].occupier == snake);
    Snake *newSnake = malloc(sizeof(Snake));
    newSnake->head.occupier = snake;
    newSnake->head.coordinate.x = x;
    newSnake->head.coordinate.y = y;
    return *newSnake;
}

void printGame(Game *game) {
    for (int i = 0; i < game->width + 2; ++i) {
        printw("#");
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
                    c = 'x';
                    break;
                default:
                    c = ' ';
            }
            printw("%c", c);
        }
        printw("#");
    }
    for (int i = 0; i < game->width + 2; ++i) {
        printw("#");
    }
    refresh();
    getch();
}

int main(int argc, char* argv[]) {
    Game *game = malloc(sizeof(Game));
    initialiseRandomSeed();
    if (argc != 3) {
        printf("Call game with height and width arguments.\n Use $LINES and $COLUMNS to find them\n");
        exit(EXIT_FAILURE);
    } else {
        game->width = strtol(argv[2], NULL, 10) - 2;
        game->height = strtol(argv[1], NULL ,10) - 2;
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
    initscr();
    printGame(game);
    endwin();
    return 0;
}

