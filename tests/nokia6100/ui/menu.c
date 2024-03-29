
#include <stdlib.h>
#include "s1d15g00.h"
#include "fonts.h"

#include "main.h"
#include "ui.h"

#include "buttons.h"

// Provide ASSERT() macro
#include "util.h"

static const menu_item_t config_item4 = {
    .text = "Config Item 4",
    .enter = NULL,
    .enter_callback = NULL,
    .next = NULL
  },
  config_item3 = {
    .text = "Config Item 3",
    .enter = NULL,
    .enter_callback = NULL,
    .next = &config_item4
  },
  config_item2 = {
    .text = "Config Item 2",
    .enter = NULL,
    .enter_callback = NULL,
    .next = &config_item3
  },
  config_item1 = {
    .text = "Config Item 1",
    .enter = NULL,
    .enter_callback = NULL,
    .next = &config_item2
  };

static const menu_item_t root_item_info = {
  .text = "Info",
  .enter = NULL,
  .enter_callback = info_callback,
  .next = NULL
},
  root_item_configuration = {
    .text = "Configuration",
    .enter = &config_item1,
    .enter_callback = NULL,
    .next = &root_item_info
  },
  root_item3 = {
    .text = "File Browser",
    .enter = NULL,
    .enter_callback = file_browser_callback,
    .next = &root_item_configuration,
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
                   *up_menu_last = NULL,
                   *up_menu_top_of_screen = NULL;



static int num_items( void )
{
  switch( current_app_state ) {
    case FILE_BROWSER:
    case DO_FULL_MENU: return FULL_MENU_NUM_ITEMS;
                       break;
    case DO_HALF_MENU: return HALF_MENU_NUM_ITEMS;
                       break;
  }
  return FULL_MENU_NUM_ITEMS;
}

static inline int get_y_offset( void )
{
  switch( current_app_state ) {
    case FILE_BROWSER:
    case DO_FULL_MENU: return FULL_MENU_YOFFSET;
                       break;
    case DO_HALF_MENU: return HALF_MENU_YOFFSET;
                       break;
  }
  return FULL_MENU_YOFFSET;
}

void refresh_menu( unsigned int keys )
{
  unsigned int y_offset = 0;
  unsigned int i = 0;
  const menu_item_t *this_item = top_of_screen_item;
  menu_item_t *tmp;
  unsigned int item_count = 0;

  if( keys & BTN_DOWN ) {
    if( current_item->next != NULL )  {
      current_item = current_item->next;
        do_redraw = true;

      // If the span between top of screen item and the current item is too large, scroll down

      item_count++;
      tmp = top_of_screen_item;
      while( (tmp->next != NULL) && (tmp->next != current_item) ) {
        item_count++;
        tmp = tmp->next;
      }

      if( item_count > num_items() - 1 ) {
        top_of_screen_item = top_of_screen_item->next;
      }
    }
  } else if( keys & BTN_UP ) {
    // Find previous
    tmp = current_menu;
    if( current_item != current_menu ) {
      while( (tmp != NULL) && (tmp->next != current_item) ) tmp = tmp->next;
        do_redraw = true;

      ASSERT( tmp->next == current_item );

      if( current_item ==  top_of_screen_item ) { 
        top_of_screen_item = tmp;
      }

      current_item = tmp;
    }

  } else if( keys & BTN_GREEN ) {
    if( current_item->enter ) {
      up_menu_last = current_item;
      up_menu_top_of_screen = top_of_screen_item;
      up_menu = current_menu;

      current_menu = top_of_screen_item = current_item = current_item->enter;
      do_redraw = true;
    } else if( current_item->enter_callback ) {
      if( current_item->enter_callback( current_item ) ) return;
    }
  } else if( keys & BTN_RED ) {
    if( up_menu != NULL ) {
      current_menu = up_menu;
      top_of_screen_item = up_menu_top_of_screen;
      current_item = up_menu_last;

      up_menu = up_menu_last = up_menu_top_of_screen = NULL;

      do_redraw = true;
    }
  }

  y_offset = get_y_offset();

  if( do_redraw == true ) {
    LCDSetRect(y_offset,0,132,132,FILL,MENU_BG);

    this_item = top_of_screen_item;

    ASSERT( this_item != NULL );
    for( ; (i < num_items()) && this_item != NULL; i++ ) {

      if( this_item == current_item ) 
        LCDPutStr( this_item->text, y_offset, MENU_X, MENU_FONT, MENU_HIGHLIGHT_FG, MENU_HIGHLIGHT_BG );
      else
        LCDPutStr( this_item->text, y_offset, MENU_X, MENU_FONT, MENU_FG, MENU_BG );

      y_offset += LINE_HEIGHT;
      this_item = this_item->next;
    }
  }

  do_redraw = false;
}

