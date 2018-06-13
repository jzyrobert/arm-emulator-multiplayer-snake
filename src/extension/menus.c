#include <stdbool.h>
#include <menu.h>
#include <stdlib.h>
#include <string.h>
#include "menus.h"
#include "structs.h"
#include "config.h"

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
            case 10:
                cur_item = current_item(menu);
                break;
        }
        //For some reason KEY_ENTER isn't recognised but 10 is the actual enter key ID
        if (c == KEY_ENTER || c == 10) {
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
    int Num_choices = MAX_PLAYERS - game->players;
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
            return i + 1;
        }
    }
    return 0;
}

bool botMenu(void) {
    char *choices[] = {
            "Don't include bots",
            "Include bots"
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

int selectFromMenu(void) {
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
    cur_item = showMenu(player_menu);
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

bool servermenu() {
    char *choices[] = {
            "Play locally",
            "Start server and enable LAN connections"
    };
    ITEM **server_num;
    MENU *server_menu;
    ITEM *cur_item = NULL;
    int Num_choices = 2;
    server_num = calloc(Num_choices + 1, sizeof(ITEM *));
    for (int k = 0; k < Num_choices; ++k) {
        server_num[k] = new_item(choices[k], "");
    }
    server_num[Num_choices] = (ITEM *) NULL;

    server_menu = new_menu(server_num);
    cur_item = showMenu(server_menu);
    for (int i = 0; i < Num_choices; ++i) {
        if (cur_item == server_num[i]) {
            unpost_menu(server_menu);
            for (int l = 0; l < Num_choices + 1; ++l) {
                free_item(server_num[l]);
            }
            free_menu(server_menu);
            free(server_num);
            return (bool) i;
        }
    }
    return 0;
}