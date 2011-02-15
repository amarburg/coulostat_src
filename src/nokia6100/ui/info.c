
#include <stdlib.h>
#include "ui.h"
#include "buttons.h"
#include "fonts.h"
#include "s1d15g00.h"
#include "ui/info.h"
#include "ui/menu.h"

// NULL is a stopper in case you don't have exactly enough items
static const char *info_strings[] =
    { "CorrosionMonitor",
      "v 0.0.0",
      "A M Marburg",
      "",
      "",
      "",
      "",
      "RED to exit", 
      NULL };

void draw_info_screen( unsigned int keys )
{
  int i = 0;
  unsigned int xoffset = LCD_WIDTH;

  if( keys & (BTN_GREEN | BTN_RED) ) {
    current_ui_state = DO_HALF_MENU;
    do_redraw = true;
    return;
  }

  if( do_redraw ) {
    LCDFillScreen( MENU_BG );
    for( i = 0; (i < FULL_MENU_NUM_ITEMS)&&(info_strings[i]!=NULL); i++ ) {
      xoffset = (LCD_WIDTH - 8 * strlen(info_strings[i])) >> 1;

      LCDPutStr( info_strings[i], i*LINE_HEIGHT, xoffset, MENU_FONT, MENU_FG, MENU_BG );
    }
  }

  do_redraw = false;
}

bool info_callback( const struct menu_item *caller )
{
  current_ui_state = INFO_SCREEN;
  do_redraw = true;

  return true;
}


