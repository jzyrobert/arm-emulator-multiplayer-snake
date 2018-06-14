#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <menu.h>
#include <string.h>
#include <pthread.h>
#include <zconf.h>
#include "fannF/src/include/fann.h"
#include "logic.h"
#include "structs.h"
#include "server.h"
#include "config.h"
#include "menus.h"

extern int list_s;
extern char **global_ips;
extern int global_ip_num;

Direction upDir = {0,-1,0,head_up};
Direction rightDir = {1,0,1,head_right};
Direction downDir = {0,1,2,head_down};
Direction leftDir = {-1,0,3,head_left};
// we use head_up as the default direction Occupier
Direction noDir = {0,0,0,head_up};

void setGameVariables(Game *game) {
    game->snakes = calloc(MAX_PLAYERS, sizeof(Snake *));
    game->food = 0;
    //% of screen to be covered in food
    game->foodAmount = (game->height * game->width) / FOOD_PERCENTAGE;
}

void loadNN(Game *game) {
    //loads neural net
    if (game->AI) {
        game->ANN = fann_create_from_file("ANN.net");
    }
}

void getTerminalSize(int *height, int *width) {
    getmaxyx(stdscr, *height, *width);
}

void initialiseColors(void) {
    start_color();
    int background = COLOR_BLACK;
    //Assiging custom RGB colour to orange
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
}

void initialiseCurses(void) {
    if (initscr() == NULL) {
        printf("Window failure!");
        exit(EXIT_FAILURE);
    }
    //Allows usage of all keyboard keys
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    //Allows input without using enter, and times out after 1 ms
    noecho();
    cbreak();
    timeout(1);
}

void addSnakes(Game *game) {
    //Snakes are added in reverse order for fallthrough convenience. snake 0 is always the last snake.
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
            printNoPlayers();
            endwin();
            freeEverything(game);
            exit(EXIT_FAILURE);
    }
    //Make every snake STARTING LENGTH longer.
    int x = STARTING_LENGTH;
    for (int j = 0; j < game->noOfSnakes; ++j) {
        for (int i = 0; i < x; ++i) {
            addLength(game, game->snakes[j]);
        }
    }
}

void buildGrid(Game *game) {
    game->grid = calloc(game->height, sizeof(Cell *));
    if (game->grid == NULL) {
        printf("Allocation failure!\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < game->height; ++i) {
        game->grid[i] = calloc(game->width, sizeof(Cell));
    }
    for (int i = 0; i < game->height; i++) {
        for (int j = 0; j < game->width; j++) {
            game->grid[i][j].occupier = nothing;
            game->grid[i][j].coordinate.x = j;
            game->grid[i][j].coordinate.y = i;
        }
    }
}

//Generates random seed
void initialiseRandomSeed(void) {
    srand(time(NULL));
}

void addSnake(Game *game,int up, int down, int left, int right) {
    int x = 0;
    int y = 0;
    getTerminalSize(&y, &x);
    Snake *newSnake = malloc(sizeof(Snake));
    game->snakes[game->noOfSnakes] = newSnake;
    newSnake->body = malloc(game->width * game->height * sizeof(Cell *));
    //We place every snake evenly apart on the game screen at the start
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

//Prints the correspondingly direction and colour head character for a snake
void findHead(Game *pGame, int j, int i) {
    for (int k = 0; k < pGame->noOfSnakes; ++k) {
        if (pGame->snakes[k]->head == &pGame->grid[j][i] && pGame->snakes[k]->alive) {
            attron(COLOR_PAIR((2 + pGame->noOfSnakes) - k));
            break;
        }
    }
}

//Prints corresponding colour body character for a snake
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
    //Each refresh we print from the top left again
    getmaxyx(stdscr, game->tHeight, game->tWidth);
    if (game->width > (game->tWidth - 2) || game->height > (game->tHeight - 2)) {
        endwin();
        printf("Error: Detected terminal resizing below game size!\n");
        exit(EXIT_FAILURE);
    }
    move(0,0);
    attron(COLOR_PAIR(1));
    for (int i = 0; i < game->width + 2; ++i) {
        printw("#");
    }
    for (int j = 0; j < game->height; ++j) {
        move(1 + j,0);
        printw("#");
        for (int i = 0; i < game->width; ++i) {
            char c;
            switch (game->grid[j][i].occupier) {
                case nothing:
                    c = ' ';
                    break;
                case head_up:
                    findHead(game, j, i);
                    c = '^';
                    break;
                case head_down:
                    findHead(game, j, i);
                    c = 'v';
                    break;
                case head_left:
                    findHead(game, j, i);
                    c = '<';
                    break;
                case head_right:
                    findHead(game, j, i);
                    c = '>';
                    break;
                case snake_body:
                    findBody(game, j, i);
                    c = 'o';
                    break;
                case dead_snake:
                    c = 'x';
                    break;
                case food:
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
        attron(COLOR_PAIR(1));
    }
    move(game->height+1,0);
    for (int i = 0; i < game->width + 2; ++i) {
        printw("#");
    }
    refresh();
}

//Allows adding of num foods
void addFoods(Game *game, int num) {
    for (int i = 0; i < num; ++i) {
        addFood(game);
    }
}

//calculates time difference in milliseconds between t0 and t1
float timedifference_msec(struct timeval t0, struct timeval t1)
{
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}

//Frees the grid, each snake and then the struct game
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

void endgame(Game *game) {
    int x;
    int y;
    int ch;
    int l = STARTING_LENGTH;
    getTerminalSize(&y, &x);
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
    //Loop to not exit until x is pressed
    while ((ch = getch()) != 'x') {

    }
}

//Collects all empty cells and assigns food to a random location
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

//Takes the input and changes the corresponding snakes direction
void updateDir(int ch, Game *pGame) {
    if (ch == 'x') {
        pGame->finished = true;
    } else {
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
}

//Returns true if opposite dir (up/down left/right)
bool oppositeDir(Snake *pSnake, Direction newDirection) {
    Direction oldDirection = pSnake->direction;
    return oldDirection.xOffset != 0 ? oldDirection.yOffset + newDirection.yOffset == 0 :
           oldDirection.xOffset + newDirection.xOffset == 0;
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

//Gets the next cell in the next direction of a snakes head
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

//Turns a dead snake into food, optionally obstacles
void killSnake(Game *game, Snake *snake) {
    snake->alive = false;
    if (DEAD_SNAKES_ARE_OBSTACLES) {
        snake->head->occupier = getHeadChar(snake);
    } else {
        snake->head->occupier = food;
    }
    game->food++;
    for (int i = 0; i < snake->length; ++i) {
        if (DEAD_SNAKES_ARE_OBSTACLES) {
            snake->body[i]->occupier = dead_snake;
        } else {
            snake->body[i]->occupier = food;
        }
        game->food++;
    }
}

//Adjusts a snake during its next move
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

//Moves the body and head long
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

//Moves the body but keeps the last cell as new end of tail
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

//For convenience, gets next cell and makes snake increase size
void addLength(Game *game, Snake *theSnake) {
    Cell *next = getNextCell(game, theSnake);
    moveSizeIncrease(game, theSnake, next);
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

//Gets cell N ahead in next direction
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

//Rob's dummy AI, snake will choose path of least resistance starting from a distance chech of CHECK cells ahead
void calculateNextMove(Game *game, Snake *pSnake, int check) {
    while (check > 0) {
        int availableMoves = 0;
        int movesID[3];
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

//Gets next cell in a certain direction (0-3)
Cell * nextDirectionCell(Game *pGame, Cell *pCell, int i) {
    Direction dir = getDirection(i);
    int nextX = (pCell->coordinate.x + dir.xOffset + pGame->width) % pGame->width;
    int nextY = (pCell->coordinate.y + dir.yOffset + pGame->height) % pGame->height;
    return &pGame->grid[nextY][nextX];
}

//Checks whether the cell is in the array
bool notIn(Cell **pCell, Cell *next, int n) {
    for (int i = 0; i < n; ++i) {
        if (pCell[i] == next) {
            return false;
        }
    }
    return true;
}

//Breadth first search of the nearest food, ignoring obstacles. If none found (dead end) then returns the snakes head
Cell * nearestFoodCell(Game *game, Cell *start) {
    //start a breadth first search
    Cell **seen = malloc(game->width * game->height * sizeof(Cell *));
    Cell **queue = malloc(game->width * game->height * sizeof(Cell *));
    int n = 0;
    int q = 0;
    int qc = 0;
    Cell *current = start;
    while (current && (current->occupier != food)) {
        seen[n] = current;
        n++;
        for (int i = 0; i < 4; ++i) {
            Cell *next = nextDirectionCell(game, current, i);
            if (notIn(seen, next, n) && notIn(queue, next, q) && (next->occupier == nothing || next->occupier == food)) {
                queue[q] = next;
                q++;
            }
        }
        current = queue[qc];
        qc++;
    }
    free(seen);
    free(queue);
    if (current) {
        return current;
    } else {
        return start;
    }
}

//Returns -1/0/1 (eg whether move is left/forwards/right) based on new to old direction
int getMoveID(Direction old, Direction new) {
    switch (old.dir) {
        case 0:
            switch (new.dir) {
                case 1:
                    return 1;
                case 3:
                    return -1;
                default:
                    return 0;
            }
        case 1:
            switch (new.dir) {
                case 0:
                    return -1;
                case 2:
                    return 1;
                default:
                    return 0;
            }
        case 2:
            switch (new.dir) {
                case 1:
                    return -1;
                case 3:
                    return 1;
                default:
                    return 0;
            }
        case 3:
            switch (new.dir) {
                case 0:
                    return 1;
                case 2:
                    return -1;
                default:
                    return 0;
            }
    }
    return 0;
}

//Calculates normalised angle (-1 to 1) of cell food to pCell, in regards to direction of the snake
double angleBasedOnDirection(Game *game, Snake *pSnake, Cell *pCell, Cell *food) {
    double angle = 0;
    int xd, yd;
    int xd1 = food->coordinate.x - pCell->coordinate.x;
    if (pCell->coordinate.x < food->coordinate.x) {
        int xd2 = food->coordinate.x - (pCell->coordinate.x + game->width);
        xd = abs(xd1) < abs(xd2) ? xd1 : xd2;
    } else {
        int xd2 = (food->coordinate.x + game->width) - pCell->coordinate.x;
        xd = abs(xd1) < abs(xd2) ? xd1 : xd2;
    }
    int yd1 = -(food->coordinate.y - pCell->coordinate.y);
    //because y grows downwards but we want to calculate angle as if it went upwards
    if (pCell->coordinate.y < food->coordinate.y) {
        int yd2 = (pCell->coordinate.y + game->height) - food->coordinate.y;
        yd = abs(yd1) < abs(yd2) ? yd1 : yd2;
    } else {
        int yd2 = pCell->coordinate.y - (food->coordinate.y + game->height);
        yd = abs(yd1) < abs(yd2) ? yd1 : yd2;
    }
    switch (pSnake->direction.dir) {
        case 0:
            angle = atan2(xd, yd);
            break;
        case 1:
            angle = atan2(-yd, xd);
            break;
        case 2:
            angle = atan2(-xd, -yd);
            break;
        case 3:
            angle = atan2(yd, -xd);
            break;
    }
    return angle / M_PI;
}

void calcFann(Game *pGame, Snake *pSnake) {
    float max = -1;
    int c = 0;
    for (int i = -1; i < 2; ++i) {
        fann_type *calc_out;
        fann_type input[8];
        int n = 0;
        for (int j = -1; j < 2; ++j) {
            pSnake->nextDir = getDirection((pSnake->direction.dir + j + 4) % 4);
            input[n] = getNextCell(pGame, pSnake)->occupier != nothing && getNextCell(pGame, pSnake)->occupier != food;
            n++;
        }
        for (int k = -1; k < 2; ++k) {
            pSnake->nextDir = getDirection((pSnake->direction.dir + k + 4) % 4);
            input[n] = getNextCell(pGame, pSnake)->occupier == food;
            n++;
        }
        pSnake->nextDir = getDirection((pSnake->direction.dir + i + 4) % 4);
        Cell *food = nearestFoodCell(pGame, pSnake->head);
        double angle = angleBasedOnDirection(pGame, pSnake, pSnake->head, food);
        input[6] = (fann_type) angle;
        input[7] = getMoveID(pSnake->direction, pSnake->nextDir);
        calc_out = fann_run(pGame->ANN, input);
        if (calc_out[0] > max) {
            max = calc_out[0];
            c = i;
        } else if (calc_out[0] == max) {
            if (rand() % 2) {
                c = i;
            }
        }
    }
    pSnake->nextDir = getDirection((pSnake->direction.dir + c + 4) % 4);
}

void updateGame(Game *game) {
    int dead = 0;
            //bots will try not to hit things
        for (int m = 0; m < game->noOfBots; ++m) {
            if (game->AI) {
                calcFann(game, game->snakes[m]);
            } else {
                calculateNextMove(game, game->snakes[m], CHECK_LENGTH);
            }
        }
    //check all tails and heads
    for (int j = 0; j < game->noOfSnakes; ++j) {
        if (game->snakes[j]->alive) {
            game->snakes[j]->nextCell = getNextCell(game, game->snakes[j]);
            for (int i = j - 1; i >= 0 ; --i) {
                if (game->snakes[i]->alive) {
                    //2 Snakes will die if they collide at an angle
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
    //Adjust alive snakes
    for (int i = 0; i < game->noOfSnakes; ++i) {
        if (game->snakes[i]->alive) {
            updateSnake(game, game->snakes[i]);
        } else {
            dead++;
        }
    }
    //If more than 1 snake, we end game when there is 1 snake left to make games faster.
    if ((QUICKER_MULTIPLAYER && game->noOfSnakes > 1 && dead >= (game->noOfSnakes - 1)) || (game->noOfSnakes == dead)) {
        game->finished = true;
    }
    //Replenishes food back to the amount required.
    if (game->food < game->foodAmount) {
        addFoods(game, game->foodAmount - (game->food));
    }
}
