/** @file   menus.h
    @author M. P. Hayes, UCECE
    @date   19 February 2009
    @brief  Menu support.

    Lifted heavily from Michael Hayes' Treetap7 menuing code.
*/

#ifndef MENUS_H
#define MENUS_H

#include "menu.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(ARRAY) (sizeof(ARRAY) / sizeof (ARRAY[0]))
#endif

typedef enum {MENU_STYLE_ROTATE, MENU_STYLE_SCROLL} menu_style_t;

#define MENU_ITEM(NAME, ACTION) {NAME, ACTION}
#define MENU(NAME, MENU_ITEMS) {NAME, MENU_ITEMS, ARRAY_SIZE (MENU_ITEMS), 0, 0, 0}


#define MENU_DEFINE(TITLE, NAME) 
#define MENU_END 
#define MENU_ITEM_ACTION(NAME, ACTION) \
static bool ACTION (void);
#define MENU_ITEM_SUBMENU(NAME, SUBMENU) \
static bool SUBMENU ## _menu_do (void);

#include "menus.def"

#undef MENU_DEFINE
#undef MENU_END
#undef MENU_ITEM_ACTION
#undef MENU_ITEM_SUBMENU


/* Define menu item lists.  */

#define MENU_DEFINE(TITLE, NAME) \
    static menu_item_t NAME ## _menu_items[] = \
{
#define MENU_END };
#define MENU_ITEM_ACTION(NAME, ACTION) \
   MENU_ITEM (NAME, ACTION),
#define MENU_ITEM_SUBMENU(NAME, SUBMENU) \
   MENU_ITEM (NAME, SUBMENU ## _menu_do),

#include "menus.def"

#undef MENU_DEFINE
#undef MENU_END
#undef MENU_ITEM_ACTION
#undef MENU_ITEM_SUBMENU


/* Define menus.  */

#define MENU_DEFINE(TITLE, NAME) \
static menu_t NAME ## _menu = MENU (TITLE, NAME ## _menu_items);
#define MENU_END
#define MENU_ITEM_ACTION(NAME, ACTION)
#define MENU_ITEM_SUBMENU(NAME, SUBMENU)

#include "menus.def"

#undef MENU_DEFINE
#undef MENU_END
#undef MENU_ITEM_ACTION
#undef MENU_ITEM_SUBMENU


/* Define functions to display submenus.  */

#define MENU_DEFINE(TITLE, NAME) 
#define MENU_END
#define MENU_ITEM_ACTION(NAME, ACTION)
#define MENU_ITEM_SUBMENU(NAME, SUBMENU)  \
static bool SUBMENU ## _menu_do (void)    \
{                                         \
    return true; /*menu_display (&SUBMENU ## _menu);*/   \
}

#include "menus.def"

#undef MENU_DEFINE
#undef MENU_END
#undef MENU_ITEM_ACTION
#undef MENU_ITEM_SUBMENU

#endif
