
#ifndef __UI_H__
#define __UI_H__

#include <stdbool.h>

#include "fonts.h"

#ifdef __cplusplus
extern "C" {
#endif

  // This is shared between a couple of modes (menus, 
  // file browser)
typedef struct menu_item {
  const char *text;
  bool (*enter_callback)( const struct menu_item *item );
  const struct menu_item *next, *enter;
} menu_item_t;

 extern bool do_redraw;

#ifdef __cplusplus
}
#endif

#include "ui/info.h"
#include "ui/menu.h"
#include "ui/file_browser.h"
#include "ui/adc.h"


#endif
