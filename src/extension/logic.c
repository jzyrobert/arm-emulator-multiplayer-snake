#include "logic.h"
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

enum snake_direction {
    up,
    right,
    down,
    left
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
    Cell *nextCell;
    Cell **body;
    int length;
    enum snake_direction nextDir;
    int direction;
    int up;
    int down;
    int left;
    int right;
    bool alive;
    bool toDie;
};

struct game {
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
    bool finished;
};

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
    getmaxyx(stdscr, y, x);
    Snake *newSnake = malloc(sizeof(Snake));
    game->snakes[game->noOfSnakes] = newSnake;
    newSnake->body = malloc(game->width * game->height * sizeof(Cell *));
    newSnake->head = &game->grid[y - 5][x/(game->players + game->noOfBots + 1) * (game->players + game->noOfBots - game->noOfSnakes)];
    newSnake->head->occupier = head_up;
    newSnake->length = 0;
    newSnake->direction = 0;
    newSnake->nextDir = up;
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

int botnumMenu(Game *game) {
    char *choices[] = {
            "1 bot",
            "2 bots",
            "3 bots",
            "4 bots",
            "5 bots",
            "6 bots",
    };
    ITEM **bot_num;
    MENU *bot_menu;
    ITEM *cur_item = NULL;
    int c;
    int Num_choices = MAX_PLAYERS - game->players;
    bot_num = calloc(Num_choices + 1, sizeof(ITEM *));
    for (int k = 0; k < Num_choices; ++k) {
        bot_num[k] = new_item(choices[k], "");
    }
    bot_num[Num_choices] = (ITEM *) NULL;

    bot_menu = new_menu(bot_num);
    mvprintw(LINES - 2, 2, "X to Exit");
    post_menu(bot_menu);
    refresh();
    while ((c = getch()) != 'x') {
        switch (c) {
            case KEY_DOWN:
                menu_driver(bot_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(bot_menu, REQ_UP_ITEM);
                break;
            case 10:
                cur_item = current_item(bot_menu);
                break;
        }
        if (c == KEY_ENTER || c == 10) {
            break;
        }
    }
    for (int i = 0; i < Num_choices; ++i) {
        if (cur_item == bot_num[i]) {
            unpost_menu(bot_menu);
            for (int l = 0; l < Num_choices + 1; ++l) {
                free_item(bot_num[l]);
            }
            free_menu(bot_menu);
            free(bot_num);
            return i + 1;
        }
    }
    return 0;
}

bool botMenu() {
    char *choices[] = {
            "Don't include bots",
            "Include bots (random moves)"
    };
    ITEM **bot_num;
    MENU *bot_menu;
    ITEM *cur_item = NULL;
    int c;
    int Num_choices = 2;
    bot_num = calloc(Num_choices + 1, sizeof(ITEM *));
    for (int k = 0; k < Num_choices; ++k) {
        bot_num[k] = new_item(choices[k], "");
    }
    bot_num[Num_choices] = (ITEM *) NULL;

    bot_menu = new_menu(bot_num);
    mvprintw(LINES - 2, 2, "X to Exit");
    post_menu(bot_menu);
    refresh();
    while ((c = getch()) != 'x') {
        switch (c) {
            case KEY_DOWN:
                menu_driver(bot_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(bot_menu, REQ_UP_ITEM);
                break;
            case 10:
                cur_item = current_item(bot_menu);
                break;
        }
        if (c == KEY_ENTER || c == 10) {
            break;
        }
    }
    for (int i = 0; i < Num_choices; ++i) {
        if (cur_item == bot_num[i]) {
            unpost_menu(bot_menu);
            for (int l = 0; l < Num_choices + 1; ++l) {
                free_item(bot_num[l]);
            }
            free_menu(bot_menu);
            free(bot_num);
            return (bool) i;
        }
    }
    return 0;
}

int selectFromMenu() {
    char *choices[] = {
            "1 Player (Arrow keys) - Green",
            "2 Players (WASD) - Blue",
            "3 Players (TFGH) - Cyan",
            "4 Players (IJKL) - Magenta",
            "5 Players ([;'#) - Red",
            "6 Players (5123) - Orangey Brown",
            "7 Players (Home/Del/End/PgDown) - Yellow",
    };
    ITEM **player_num;
    MENU *player_menu;
    ITEM *cur_item = NULL;
    int c;
    int Num_choices = MAX_PLAYERS;
    player_num = calloc(Num_choices + 1, sizeof(ITEM *));
    for (int k = 0; k < Num_choices; ++k) {
        player_num[k] = new_item(choices[k], "");
    }
    player_num[Num_choices] = (ITEM *)NULL;

    player_menu = new_menu(player_num);
    char *title[] = {
            "         _______..__   __.      ___       __  ___  _______    ____    __    ____  ___      .______          _______.",
            "        /       ||  \\ |  |     /   \\     |  |/  / |   ____|   \\   \\  /  \\  /   / /   \\     |   _  \\        /       |",
            "       |   (----`|   \\|  |    /  ^  \\    |  '  /  |  |__       \\   \\/    \\/   / /  ^  \\    |  |_)  |      |   (----`",
            "        \\   \\    |  . `  |   /  /_\\  \\   |    <   |   __|       \\            / /  /_\\  \\   |      /        \\   \\    ",
            "    .----)   |   |  |\\   |  /  _____  \\  |  .  \\  |  |____       \\    /\\    / /  _____  \\  |  |\\  \\----.----)   |",
            "    |_______/    |__| \\__| /__/     \\__\\ |__|\\__\\ |_______|       \\__/  \\__/ /__/     \\__\\ | _| `._____|_______/"
    };
    for (int j = 0; j < 6; ++j) {
        mvprintw(LINES/2 + j,0,title[j]);
    }
    mvprintw(LINES - 2, 2, "X to Exit");
    post_menu(player_menu);
    refresh();
    while ((c = getch()) != 'x') {
        switch(c) {
            case KEY_DOWN:
                menu_driver(player_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(player_menu, REQ_UP_ITEM);
                break;
            case 10:
                cur_item = current_item(player_menu);
                break;
        }
        if (c == KEY_ENTER || c == 10) {
            break;
        }
    }
    for (int i = 0; i < Num_choices; ++i) {
        if (cur_item == player_num[i]) {
            unpost_menu(player_menu);
            for (int l = 0; l < Num_choices + 1; ++l) {
                free_item(player_num[l]);
            }
            free_menu(player_menu);
            free(player_num);
            return i + 1;
        }
    }
    unpost_menu(player_menu);
    for (int l = 0; l < Num_choices + 1; ++l) {
        free_item(player_num[l]);
    }
    free_menu(player_menu);
    free(player_num);
    return 0;
}

void printNoPlayers() {
    int x;
    int y;
    getmaxyx(stdscr, y, x);
    clear();
    char msg1[] = "No players selected";
    char msg2[] = "Press any button to exit";
    mvprintw(y/2, x/2 - strlen(msg1) / 2, "%s",msg1);
    mvprintw(y/2 + 1, x/2 - strlen(msg2) / 2, "%s",msg2);
    refresh();
    nodelay(stdscr, false);
    getch();
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
    timeout(1);

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

    game->players = selectFromMenu();

    if (game->players > 0) {
        if (botMenu()) {
            game->noOfBots = botnumMenu(game);
            if (game->noOfBots == 0) {
                endwin();
                freeEverything(game);
                exit(EXIT_FAILURE);
            }
        } else {
            game->noOfBots = 0;
        }
    }

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
    int x = STARTING_LENGTH;
    for (int j = 0; j < game->noOfSnakes; ++j) {
        for (int i = 0; i < x; ++i) {
            addLength(game, game->snakes[j]);
        }
    }
    int ch;
    printGame(game);
    usleep(300000);
    struct timeval start, next;
    gettimeofday(&start, 0);
    float elapsed = 0;
    int speed = 100;
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

void updateDir(int ch, Game *pGame) {
    if (ch == 'x') {
        pGame->finished = true;
    } else {
        for (int i = 0; i < pGame->noOfSnakes; ++i) {
            int direction = -1;
            if (ch == pGame->snakes[i]->up) {
                direction = up;
            }
            if (ch == pGame->snakes[i]->right) {
                direction = right;
            }
            if (ch == pGame->snakes[i]->down) {
                direction = down;
            }
            if (ch == pGame->snakes[i]->left) {
                direction = left;
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
        case up:
            return  0;
        case right:
            return 1;
        case down:
            return 0;
        case left:
            return -1;
        default:
            return 0;
    }
}

int getYOffset(Snake *snake) {
    switch (snake->nextDir) {
        case up:
            return -1;
        case right:
            return 0;
        case down:
            return 1;
        case left:
            return 0;
        default:
            return 0;
    }
}

enum OCCUPIER getHeadChar(Snake *theSnake){
    switch (theSnake->nextDir) {
        case up:
            return head_up;
        case right:
            return head_right;
        case down:
            return head_down;
        case left:
            return head_left;
        default:
            return head_up;
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

void killSnake(Game *game, Snake *snake) {
    snake->alive = false;
    //snake->head->occupier = food;
            snake->head->occupier = getHeadChar(snake);
    game->food++;
    for (int i = 0; i < snake->length; ++i) {
        //snake->body[i]->occupier = food;
        snake->body[i]->occupier = dead_snake;
        game->food++;
    }
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
    static int moves = 0;
    //assign a random direction to all bots randomly
            //bots will try not to hit things
        for (int m = 0; m < game->noOfBots; ++m) {
            //move if about to hit or every 3rd square?
            if ((getNextCell(game, game->snakes[m])->occupier != nothing && getNextCell(game, game->snakes[m])->occupier != food) || !(moves % 3)) {
                int roll, n = 0;
                do {
                    roll = rand() % 4;
                    game->snakes[m]->nextDir = roll;
                    n++;
                } while ((n < 1000) && (abs(roll - game->snakes[m]->direction) == 2) && (getNextCell(game, game->snakes[m])->occupier != nothing && getNextCell(game, game->snakes[m])->occupier != food));
            }
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
    if (dead == game->noOfSnakes) {
        game->finished = true;
    }
    if (game->food < game->foodAmount) {
        addFoods(game, game->foodAmount - (game->food));
    }
}

