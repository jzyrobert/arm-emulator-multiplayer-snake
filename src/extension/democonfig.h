#ifndef ARM11_01_CONFIG_H
#define ARM11_01_CONFIG_H
//Placeholder so GCC recompiles on changes here but does not complain
typedef int placeholder;
#define MAX_PLAYERS 7
#define STARTING_LENGTH 4
#define COLOR_ORANGE 8
//Check length for dummy AI movement
#define CHECK_LENGTH 200
//Total size / percentage, eg 1 = 100%, 100 = 1%;
#define FOOD_PERCENTAGE 10
//in ms
#define GAME_REFRESH_SPEED 10
#define PORT 2035
//If 1, dead snakes become obstacles
#define DEAD_SNAKES_ARE_OBSTACLES 0
//If 1, games end when there is 1 player left, IF it started with more than 1 player
#define QUICKER_MULTIPLAYER 0

#endif //ARM11_01_CONFIG_H
