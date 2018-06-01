#include <stdbool.h>

#ifndef GAME_LOGIC_DEFINITIONS
#define GAME_LOGIC_DEFINITIONS
typedef struct coordinate Coordinate;
typedef struct cell Cell;
typedef struct snake Snake;
typedef struct game Game;
typedef struct direction Direction;

void printGame(Game *pGame);
                                                                                
void updateGame(Game *game);
                                                                                
void updateDir(int ch, Game *pGame);
                                                                                
void moveSnake(Game *game, Snake *theSnake, Cell *next);
                                                                                
void addLength(Game *game, Snake *theSnake);
                                                                                
bool oppositeDir(Snake *pSnake, Direction newDirection);
                                                                                
void moveSizeIncrease(Game *game, Snake *theSnake, Cell *next);
                                                                                
void addFood(Game *pGame);
                                                                                
void endgame(Game *game);

// Initialises the random seed used to add snakes to random positions on the grid
void initialiseRandomSeed(void);
#endif
