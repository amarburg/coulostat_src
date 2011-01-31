


#include "ui.h"
#include "buttons.h"

#include <stdlib.h>
#include <stdbool.h>

#include "s1d15g00.h"
#include "libmaple.h"
#include "util.h"

typedef enum ui_state {
  DO_MENU_FULL,
  DO_MENU_HALF
} ui_state_t;

static ui_state_t current_ui_state = DO_MENU_FULL;

typedef struct menu_item {
  const char *text;
  bool (*enter_callback)( void );
  const struct menu_item *next, *enter;
} menu_item_t;

static const menu_item_t info_item4 = {
    .text = "Info Item 4",
    .enter = NULL,
    .enter_callback = NULL,
    .next = NULL
  },
  info_item3 = {
    .text = "Info Item 3",
    .enter = NULL,
    .enter_callback = NULL,
    .next = &info_item4
  },
  info_item2 = {
    .text = "Info Item 2",
    .enter = NULL,
    .enter_callback = NULL,
    .next = &info_item3
  },
  info_item1 = {
    .text = "Info Item 1",
    .enter = NULL,
    .enter_callback = NULL,
    .next = &info_item2
  };

static const menu_item_t root_info = {
  .text = "Info",
  .enter = &info_item1,
  .enter_callback = NULL,
  .next = NULL
},
  root_item4 = {
    .text = "Item 4",
    .enter = NULL,
    .enter_callback = NULL,
    .next = &root_info
  },
  root_item3 = {
    .text = "Item 3",
    .enter = NULL,
    .enter_callback = NULL,
    .next = &root_item4
  },
  root_item2 = {
    .text = "Item 2",
    .enter = NULL,
    .enter_callback = NULL,
    .next = &root_item3
  },
  root_item1 = {
    .text = "Item 1",
    .enter = NULL,
    .enter_callback = NULL,
    .next = &root_item2
  };

static const menu_item_t *current_item = &root_item1, 
                   *top_of_screen_item = &root_item1,
                   *current_menu = &root_item1,
                   *up_menu   = NULL,
                   *up_menu_last = NULL;


#define FULL_MENU_NUM_ITEMS 8

#define HALF_MENU_YOFFSET 65
#define HALF_MENU_NUM_ITEMS (FULL_MENU_NUM_ITEMS/2)

#define MENU_FG WHITE
#define MENU_BG BLACK
#define MENU_HIGHLIGHT_FG BLACK
#define MENU_HIGHLIGHT_BG GREEN
#define MENU_FONT LARGE
#define LINE_HEIGHT 16
#define MENU_X 0

void refresh_ui( void )
{
  unsigned int y_offset = 0;
  unsigned int num_items = 8;
  unsigned int i = 0;
  menu_item_t *this_item = top_of_screen_item;

  if( current_ui_state == DO_MENU_HALF ) {
    y_offset = HALF_MENU_YOFFSET;
    num_items = HALF_MENU_NUM_ITEMS;
  }

  ASSERT( this_item != NULL );
  for( ; (i < num_items) && this_item != NULL; i++ ) {

    if( this_item == current_item ) 
      LCDPutStr( this_item->text, y_offset,MENU_X, MENU_FONT, MENU_HIGHLIGHT_FG, MENU_HIGHLIGHT_BG );
    else
      LCDPutStr( this_item->text, y_offset, MENU_X, MENU_FONT, MENU_FG, MENU_BG );

    y_offset += LINE_HEIGHT;
    this_item = this_item->next;
  }
  LCDSetRect(y_offset,0,132,132,FILL,MENU_BG);


}

void ui_keypress( unsigned int keys )
{
  menu_item_t *tmp;

  if( keys & BTN_DOWN ) {
    if( current_item->next != NULL )  {
      current_item = current_item->next;
    }
  } else if( keys & BTN_UP ) {
    // Find previous
    tmp = current_menu;
    if( current_item != current_menu ) {
      while( (tmp != NULL) && (tmp->next != current_item) ) tmp = tmp->next;

      ASSERT( tmp->next == current_item );

      if( current_item ==  top_of_screen_item ) top_of_screen_item = tmp;
      current_item = tmp;
    }

  } else if( keys & BTN_GREEN ) {
    if( current_item->enter ) {
      up_menu_last = current_item;
      up_menu = current_menu;

      current_menu = top_of_screen_item = current_item = current_item->enter;
    }
  } else if( keys & BTN_RED ) {
    if( up_menu != NULL ) {
      current_menu = up_menu;
      //TODO This isn't quite right, actually...
      top_of_screen_item = up_menu;

      if( up_menu_last ) 
        current_item = up_menu_last;
      else
        current_item = up_menu;
    }
  }


}


