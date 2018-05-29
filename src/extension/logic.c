#include <stdio.h>
#include "logic.h"

#ifndef GRID_SIZE
#define GRID_SIZE 8192
#endif

typedef enum {
    false, true
} bool;

enum OCCUPIER {
    nothing,
    snake,
    dead_snake
};

typedef struct {
    int x;
    int y;
} Coordinate;

typedef struct {
    enum OCCUPIER occupier;
    Coordinate coordinate;
} Cell;

typedef struct {
    Cell head;
} Snake;

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

// TODO: will be entry point for game; not defined yet
int main(void) {
    return 0;
}