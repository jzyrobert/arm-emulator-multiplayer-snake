#ifndef GAME_LOGIC_DEFINITIONS
#define GAME_LOGIC_DEFINITIONS
typedef struct coordinate Coordinate;
typedef struct cell Cell;
typedef struct snake Snake;
#endif

// Initialises the random seed used to add snakes to random positions on the grid
void initialiseRandomSeed(void);

/*
// Creates an empty game grid ready for snakes to be added when the game starts
void buildGrid(Cell (*grid)[GRID_SIZE][GRID_SIZE]);

// Adds a new snake to a random position on the board. Returns this snake
// PRE: there must be at least one space not occupied by a snake on the board
Snake addSnake(Cell (*grid)[GRID_SIZE][GRID_SIZE]);
 */