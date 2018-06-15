#include <stdbool.h>

#ifndef ARM11_01_STRUCTS_H
#define ARM11_01_STRUCTS_H
typedef struct coordinate Coordinate;
typedef struct cell Cell;
typedef struct snake Snake;
typedef struct game Game;
typedef struct direction Direction;
typedef struct request Request;
typedef struct utils ServerUtils;

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
    Snake **snakes;
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

struct request{
    char* dir;
    char *msg;
    char *file;
    bool get;
    bool img;
    bool css;
    bool html;
    char* ip;
};

struct utils {
    Game* game;
    struct sockaddr_in *serverAddress;
    int* addrSize;
};

#endif //ARM11_01_STRUCTS_H
