

#ifndef _UI_MENU_H_
#define _UI_MENU_H_

#include <stdbool.h>

#define FULL_MENU_YOFFSET 2
#define FULL_MENU_NUM_ITEMS 8

#define HALF_MENU_YOFFSET 66
#define HALF_MENU_NUM_ITEMS (FULL_MENU_NUM_ITEMS/2)

#define MENU_FG WHITE
#define MENU_BG BLACK
#define MENU_HIGHLIGHT_FG BLACK
#define MENU_HIGHLIGHT_BG GREEN
#define MENU_FONT LARGE
#define LINE_HEIGHT 16
#define MENU_X 0


void refresh_menu( unsigned int keys );


#endif

