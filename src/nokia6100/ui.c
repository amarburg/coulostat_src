


#include "ui.h"
#include "buttons.h"

#include "fonts.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "libmaple.h"
#include "term_io.h"
#include "util.h"
#include "s1d15g00.h"

#include "ui/menu.h"
#include "ui/file_browser.h"
#include "ui/info.h"

ui_state_t current_ui_state = DO_HALF_MENU;
bool do_redraw = true;

void refresh_ui( unsigned int keys )
{
  switch( current_ui_state ) {
    case DO_FULL_MENU:
    case DO_HALF_MENU:
    case FILE_BROWSER:
      refresh_menu( keys);
      break;
    case INFO_SCREEN:
      draw_info_screen( keys);
      break;
  }
}


