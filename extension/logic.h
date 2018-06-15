#include <stdbool.h>
#include "structs.h"
#include <time.h>

#ifndef GAME_LOGIC_DEFINITIONS
#define GAME_LOGIC_DEFINITIONS
// used to handle printing of keys not part of the ASCII standard
typedef int unicode_char;

void initialiseRandomSeed(void);

void getTerminalSize(int *height, int *width);

void setGameVariables(Game *game);

void initialiseColors(void);

void initialiseCurses(void);

void printGame(Game *pGame);
                                                                                
void updateGame(Game *game);

void loadNN(Game *game);

void buildGrid(Game *game);
                                                                                
void updateDir(int ch, Game *pGame);

float timedifference_msec(struct timeval t0, struct timeval t1);

void addSnakes(Game *game);

void addSnake(Game *game,int up, int down, int left, int right);

void moveSnake(Game *game, Snake *theSnake, Cell *next);
                                                                                
void addLength(Game *game, Snake *theSnake);
                                                                                
bool oppositeDir(Snake *pSnake, Direction newDirection);
                                                                                
void moveSizeIncrease(Game *game, Snake *theSnake, Cell *next);

void addFoods(Game *game, int num);
                                                                                
void addFood(Game *pGame);

void freeEverything(Game *pGame);

void endgame(Game *game);

// Initialises the random seed used to add food to random positions on the grid
void initialiseRandomSeed(void);
#endif
