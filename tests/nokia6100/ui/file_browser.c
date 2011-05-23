

#include <stdlib.h>

#include "main.h"
#include "ui/file_browser.h"
#include "ui/menu.h"

bool file_browser_callback( const struct menu_item *caller )
{
  current_app_state = FILE_BROWSER;
  do_redraw = true;
  return true;
}

