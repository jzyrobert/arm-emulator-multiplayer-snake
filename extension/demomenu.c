#include <stdbool.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>
#include "menus.h"
#include "structs.h"
//File is a copy of menus.c, modified to use democonfig
#include "democonfig.h"

#define ENTER_KEY 10

ITEM *showMenu(MENU *menu) {
    int c;
    mvprintw(LINES - 2, 2, "X to Exit");
    ITEM *cur_item = NULL;
    post_menu(menu);
    refresh();
    while ((c = getch()) != 'x') {
        switch (c) {
            case KEY_DOWN:
                menu_driver(menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(menu, REQ_UP_ITEM);
                break;
            case ENTER_KEY:
                cur_item = current_item(menu);
                break;
        }
        //Enter keys on windows and linux are different
        if (c == KEY_ENTER || c == ENTER_KEY) {
            break;
        }
    }
    if (c == 'x') {
        endwin();
        exit(EXIT_FAILURE);
    }
    return cur_item;
}

bool annMenu(void) {
    char *choices[] = {
            "Bots use Rob's dummy AI to move",
            "Bots use a trained Neural Network to move"
    };
    ITEM **bot_num;
    MENU *bot_menu;
    ITEM *cur_item = NULL;
    int Num_choices = 2;
    bot_num = calloc(Num_choices + 1, sizeof(ITEM *));
    for (int k = 0; k < Num_choices; ++k) {
        bot_num[k] = new_item(choices[k], "");
    }
    bot_num[Num_choices] = (ITEM *) NULL;

    bot_menu = new_menu(bot_num);
    cur_item = showMenu(bot_menu);
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
