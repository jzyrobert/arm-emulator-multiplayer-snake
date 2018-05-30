#include "logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#ifndef GRID_SIZE
#define GRID_SIZE 10
#endif

enum OCCUPIER {
    nothing,
    head_up,
    head_down,
    head_left,
    head_right,
    snake_body,
    dead_snake,
    food
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
    int nextDir;
    int direction;
    int up;
    int down;
    int left;
    int right;
    bool alive;
};

struct game {
    Cell **grid;
    Snake *snakes[4];
    int noOfSnakes;
    int width;
    int height;
    int tWidth;
    int tHeight;
    int food;
    time_t lastTime;
    bool finished;
};

void printGame(Game *pGame);

bool finished(Game *pGame);

void updateGame(Game *game) ;

void updateDir(int ch, Game *pGame);

void moveSnake(Game *game, Snake *theSnake, Cell *next) ;

void addLength(Game *game, Snake *theSnake) ;

bool oppositeDir(Snake *pSnake, int ch);

void endgame();

void moveSizeIncrease(Game *game, Snake *theSnake, Cell *next) ;

void addFood(Game *pGame) ;

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

void addSnake(Game *game,int up, int down, int left, int right) {
    int x = 0;
    int y = 0;
    do {
        x = rand() % (game->width + 1);
        y = rand() % (game->height + 1);
    } while (game->grid[y][x].occupier != nothing);
    Snake *newSnake = malloc(sizeof(Snake));
    game->snakes[game->noOfSnakes] = newSnake;
    newSnake->body = malloc(game->width * game->height * sizeof(Cell *));
    newSnake->head = &game->grid[y][x];
    newSnake->head->occupier = head_up;
    newSnake->length = 0;
    newSnake->direction = 0;
    newSnake->nextDir = 0;
    game->noOfSnakes++;
    newSnake->up = up;
    newSnake->down = down;
    newSnake->left = left;
    newSnake->right = right;
    newSnake->alive = true;
}

void printGame(Game *game) {
    clear();
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
            attron(COLOR_PAIR(2));
            char c;
            switch ((int) game->grid[j][i].occupier) {
                case 0:
                    c = ' ';
                    break;
                case 1:
                    c = '^';
                    break;
                case 2:
                    c = 'v';
                    break;
                case 3:
                    c = '<';
                    break;
                case 4:
                    c = '>';
                    break;
                case 5:
                    c = 'o';
                    break;
                case 6:
                    c = 'x';
                    break;
                case 7:
                    attron(COLOR_PAIR(3));
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

void addFoods(Game *game, int num) {
    for (int i = 0; i < num; ++i) {
        addFood(game);
    }
}

float timedifference_msec(struct timeval t0, struct timeval t1)
{
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

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
    keypad(stdscr, TRUE);
    noecho();
    cbreak();
    timeout(100);

    start_color();
    int backgroud = COLOR_BLACK;
    init_pair(1, COLOR_WHITE, backgroud); //Main pair is white and black
    init_pair(2, COLOR_GREEN, backgroud); //Second pair is green and black
    init_pair(3, COLOR_RED, backgroud); //Second pair is red and black

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
        if (game->width < 2 || game->height < 2) {
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
    addSnake(game, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT);
    addSnake(game, 'w', 's', 'a', 'd');
    addSnake(game, 't', 'g', 'f', 'h');
    addSnake(game, 'i', 'k', 'j', 'l');
    addFoods(game, 4);
    for (int j = 0; j < game->noOfSnakes; ++j) {
        for (int i = 0; i < 5; ++i) {
            addLength(game, game->snakes[j]);
        }
    }
    int ch;
    printGame(game);
    struct timeval start, next;
    gettimeofday(&start, 0);
    float elapsed = 0;
    while(!game->finished) {
        gettimeofday(&next, 0);
        elapsed = timedifference_msec(start, next);
        if (elapsed > 100) {
            gettimeofday(&start, 0);
            updateGame(game);
            printGame(game);
        }
        if ((ch = getch()) != ERR) {
            updateDir(ch, game);
        }
    }
    endgame();
    endwin();
    freeEverything(game);
    return 0;
}

void endgame() {
    int x;
    int y;
    getmaxyx(stdscr, y, x);
    clear();
    char msg1[] = "The game has ended!";
    char msg2[] = "Press any button to exit";
    mvprintw(y/2, x/2 - strlen(msg1) / 2, "%s",msg1);
    mvprintw(y/2 + 1, x/2 - strlen(msg2) / 2, "%s",msg2);
    refresh();
    nodelay(stdscr, false);
    getch();
}

void addFood(Game *pGame) {
    pGame->food++;
    int x = 0;
    int y = 0;
    do {
        x = rand() % (pGame->width + 1);
        y = rand() % (pGame->height + 1);
    } while (pGame->grid[y][x].occupier != nothing);
    pGame->grid[y][x].occupier = food;
}

void updateDir(int ch, Game *pGame) {
    if (ch == 'x') {
        pGame->finished = true;
    } else {
        for (int i = 0; i < pGame->noOfSnakes; ++i) {
            int direction = -1;
            if (ch == pGame->snakes[i]->up) {
                direction = 0;
            }
            if (ch == pGame->snakes[i]->right) {
                direction = 1;
            }
            if (ch == pGame->snakes[i]->down) {
                direction = 2;
            }
            if (ch == pGame->snakes[i]->left) {
                direction = 3;
            }
            if (direction != -1) {
                if (!oppositeDir(pGame->snakes[i], direction)) {
                    pGame->snakes[i]->nextDir = direction;
                }
            }
        }
    }
}

bool oppositeDir(Snake *pSnake, int ch) {
    return abs(ch - pSnake->direction) == 2;
}

int getXOffset(Snake *snake) {
    switch (snake->nextDir) {
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
    switch (snake->nextDir) {
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

enum OCCUPIER getHeadChar(Snake *theSnake){
    switch (theSnake->nextDir) {
        case 0:
            return head_up;
        case 1:
            return head_right;
        case 2:
            return head_down;
        case 3:
            return head_left;
        default:
            return head_up;
    }
}

void updateSnake(Game *game, Snake *snake) {
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
        Cell *next = &game->grid[nextY][nextX];
        if (next->occupier != nothing && next->occupier != food) {
            //rip snake
            snake->alive = false;
            snake->head->occupier = food;
            game->food++;
            for (int i = 0; i < snake->length; ++i) {
                snake->body[i]->occupier = food;
                game->food++;
            }
        } else if (next->occupier == food){
            game->food--;
            moveSizeIncrease(game, snake, next);
        } else {
            moveSnake(game, snake, next);
        }
}

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
    previous->occupier = nothing;
    theSnake->direction = theSnake->nextDir;
}

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

void addLength(Game *game, Snake *theSnake) {
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
    moveSizeIncrease(game, theSnake, next);
}

void updateGame(Game *game) {
    int dead = 0;
    for (int i = 0; i < game->noOfSnakes; ++i) {
        if (game->snakes[i]->alive) {
            updateSnake(game, game->snakes[i]);
        } else {
            dead++;
        }
    }
    if (dead == game->noOfSnakes) {
        game->finished = true;
    }
    if (game->food < 4) {
        addFoods(game, 4 - (game->food));
    }
}

