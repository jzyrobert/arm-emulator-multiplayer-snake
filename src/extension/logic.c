#include "logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#ifndef GRID_SIZE
#define GRID_SIZE 8192
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

Cell grid[GRID_SIZE][GRID_SIZE];

void buildGrid(void) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j].occupier = nothing;
            grid[i][j].coordinate.x = i;
            grid[i][j].coordinate.y = j;
        }
    }
}

void initialiseRandomSeed(void) {
    srand(time(NULL));
}

Snake addSnake(void) {
    int x = 0;
    int y = 0;
    do {
        x = rand();
        y = rand();
    } while (grid[x][y].occupier == snake);
    Snake *newSnake = malloc(sizeof(Snake));
    newSnake->head.occupier = snake;
    newSnake->head.coordinate.x = x;
    newSnake->head.coordinate.y = y;
    return *newSnake;
}

int main(void) {
    initialiseRandomSeed();
    buildGrid();
}
