

#include <stdlib.h>

#include "ui/file_browser.h"
#include "ui/menu.h"
#include "buttons.h"

bool file_browser_callback( const struct menu_item *caller )
{
  current_ui_state = FILE_BROWSER;
  do_redraw = true;
  return true;
}

