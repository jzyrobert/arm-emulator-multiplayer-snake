#include <menu.h>
#include "structs.h"

#ifndef ARM11_01_MENUS_H
#define ARM11_01_MENUS_H

ITEM *showMenu(MENU *menu);

//Chooses whether to use AI for moves
bool annMenu(void);

//Allows choosing number of bots based on number of players
int botnumMenu(Game *game);

//To include bots or not
bool botMenu(void);

//Title screen menu
int selectFromMenu(void);

//If the exit button (X) is pressed during player selection
void printNoPlayers();

bool servermenu();
#endif //ARM11_01_MENUS_H
