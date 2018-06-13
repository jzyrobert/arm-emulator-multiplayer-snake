#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <menu.h>

#ifndef GRID_SIZE
#define GRID_SIZE 10
#endif

#define MAX_PLAYERS 7
#define STARTING_LENGTH 4
#define COLOR_ORANGE 8
#define SCALE(a) a * 51 / 200
#define CHECK_LENGTH 200

enum Occupier {
    nothing,
    head_up,
    head_down,
    head_left,
    head_right,
    snake_body,
    dead_snake,
    food
};

// positive y values go down the screen
typedef struct direction {
    int xOffset;
    int yOffset;
    enum Occupier headOccupier;
} Direction;

Direction upDir = {0,-1,head_up};
Direction rightDir = {1,0,head_right};
Direction downDir = {0,1,head_down};
Direction leftDir = {-1,0,head_left};
// we use head_up as the default direction Ocuupier
Direction noDir = {0,0,head_up};

typedef struct coordinate {
    int x;
    int y;
} Coordinate;

typedef struct cell {
    enum Occupier occupier;
    Coordinate coordinate;
} Cell;

typedef struct snake {
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
} Snake;

typedef struct game {
    Cell **grid;
    Snake *snakes[MAX_PLAYERS];
    int noOfSnakes;
    int noOfBots;
    int width;
    int height;
    int tWidth;
    int tHeight;
    int food;
    int foodAmount;
    int players;
    int dead;
    bool finished;
    bool justDied;
    FILE *output;
} Game;

Direction getDirection(int choice) ;

Cell getCellAhead(Game *game, Snake *snake, int n) ;

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
    game->justDied = false;
    game->dead = 0;
}

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


void findHead(Game *pGame, int j, int i) {
    for (int k = 0; k < pGame->noOfSnakes; ++k) {
        if (pGame->snakes[k]->head == &pGame->grid[j][i] && pGame->snakes[k]->alive) {
            attron(COLOR_PAIR((2 + pGame->noOfSnakes) - k));
            break;
        }
    }
}

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
    fclose(pGame->output);
    free(pGame);
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

void killSnake(Game *game, Snake *snake) {
    for (int j = 0; j < 4; ++j) {
        snake->nextDir = getDirection(j);
        for (int i = 1; i < 100; ++i) {
            if (getCellAhead(game ,snake, i).occupier != food && getCellAhead(game, snake, i).occupier != nothing) {
                fprintf(game->output, " %d : %d ", j, i-1);
                break;
            }
        }
    }
    fprintf(game->output, "\n");
    snake->alive = false;
    snake->head->occupier = food;
    //snake->head->occupier = getHeadChar(snake);
    game->food++;
    for (int i = 0; i < snake->length; ++i) {
        snake->body[i]->occupier = food;
        //snake->body[i]->occupier = dead_snake;
        game->food++;
    }
    game->justDied = true;
    game->dead++;
}

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

bool oppositeDir(Snake *pSnake, Direction newDirection) {
    Direction oldDirection = pSnake->direction;
    return oldDirection.xOffset != 0 ? oldDirection.yOffset + newDirection.yOffset == 0 :
           oldDirection.xOffset + newDirection.xOffset == 0;
}

void calculateNextMove(Game *game, Snake *pSnake, int check) {
    while (check > 0) {
        int availableMoves = 0;
        int movesID[3] = {0,0,0};
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

void updateGame(Game *game) {
    int dead = 0;
    static int moves = 0;
    //assign a random direction to all bots randomly
    //bots will try not to hit things
    for (int m = 0; m < game->noOfBots; ++m) {
        calculateNextMove(game, game->snakes[m], CHECK_LENGTH);
        /*
        //move if about to hit or every 3rd square?
        if ((getNextCell(game, game->snakes[m])->occupier != nothing && getNextCell(game, game->snakes[m])->occupier != food) || !(moves % 3)) {
            int roll, n = 0;
            do {
                roll = rand() % 4;
                game->snakes[m]->nextDir = getDirection(roll);
                n++;
            } while ((n < 1000) && oppositeDir(game->snakes[m], game->snakes[m]->nextDir) && (getNextCell(game, game->snakes[m])->occupier != nothing && getNextCell(game, game->snakes[m])->occupier != food));
        }
        */
    }
    moves++;
    //check all tails and heads
    for (int j = 0; j < game->noOfSnakes; ++j) {
        if (game->snakes[j]->alive) {
            game->snakes[j]->nextCell = getNextCell(game, game->snakes[j]);
            for (int i = j - 1; i >= 0 ; --i) {
                if (game->snakes[i]->alive) {
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
    for (int i = 0; i < game->noOfSnakes; ++i) {
        if (game->snakes[i]->alive) {
            updateSnake(game, game->snakes[i]);
        } else {
            dead++;
        }
    }
    if (dead == game->dead) {
        game->justDied = false;
    }
    if (dead == (game->noOfSnakes - 1)) {
        game->finished = true;
    }
    if (game->food < game->foodAmount) {
        addFoods(game, game->foodAmount - (game->food));
    }
}

void updateDir(int ch, Game *pGame) {
    if (ch == 'x') {
        pGame->finished = true;
    }
    /* else {
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
     */
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
    while ((ch = getch()) != 'x') {

    }
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
    timeout(10);

    game->output = fopen("debug.txt", "w");


    start_color();
    int background = COLOR_BLACK;
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
            game->height = game->tHeight - 2;
            game->width = game->tWidth - 2;
        }
    }

    game->foodAmount = (game->height * game->width) / 10;
    game->grid = calloc(game->height, sizeof(Cell *));
    if (game->grid == NULL) {
        printf("Allocation failure!\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < game->height; ++i) {
        game->grid[i] = calloc(game->width, sizeof(Cell));
    }
    buildGrid(game);

    game->players = 0;
    game->noOfBots = 7;

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
            endwin();
            freeEverything(game);
            exit(EXIT_FAILURE);
    }

    int x = STARTING_LENGTH;
    for (int j = 0; j < game->noOfSnakes; ++j) {
        for (int i = 0; i < x; ++i) {
            addLength(game, game->snakes[j]);
        }
    }
    int ch;
    printGame(game);
    struct timeval start, next;
    gettimeofday(&start, 0);
    float elapsed = 0;
    int speed = 10;
    clear();
    while(!game->finished) {
        gettimeofday(&next, 0);
        elapsed = timedifference_msec(start, next);
        if (elapsed > speed) {
            gettimeofday(&start, 0);
            updateGame(game);
            printGame(game);
        }
        if ((ch = getch()) != ERR) {
            updateDir(ch, game);
        }
    }
    endgame(game);
    endwin();
    freeEverything(game);
    return 0;
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


