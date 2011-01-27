


#include "ui.h"

#include <stdlib.h>
#include <stdbool.h>

#include "s1d15g00.h"

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
                   *top_of_screen_item = &root_item1;


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

  assert( this_item != NULL );
  for( ; (i < num_items) && this_item != NULL; i++ ) {

    if( this_item = current_item ) 
      LCD_PutStr( this_item->text, MENU_X, y_offset, MENU_FONT, MENU_HIGHLIGHT_FG, MENU_HIGHLIGHT_BG );
    else
      LCD_PutStr( this_item->text, MENU_X, y_offset, MENU_FONT, MENU_FG, MENU_BG );

    y_offset += LINE_HEIGHT;
    this_item = this_item->next;
  }



}


