
#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef enum big_state_enum {
    DO_FULL_MENU,
    DO_HALF_MENU,
    INFO_SCREEN,
    FILE_BROWSER
  } big_state_t;

  extern big_state_t current_app_state;

  extern bool init_complete;

#ifdef __cplusplus
}
#endif

#endif
