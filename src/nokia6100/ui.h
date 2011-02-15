
#ifndef __UI_H__
#define __UI_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct menu_item {
  const char *text;
  bool (*enter_callback)( const struct menu_item *item );
  const struct menu_item *next, *enter;
} menu_item_t;

  typedef enum ui_state {
    DO_FULL_MENU,
    DO_HALF_MENU,
    INFO_SCREEN,
    FILE_BROWSER
  } ui_state_t;

  extern ui_state_t current_ui_state;
  extern bool do_redraw;

  void refresh_ui( unsigned int keys );

#ifdef __cplusplus
}
#endif

#endif
