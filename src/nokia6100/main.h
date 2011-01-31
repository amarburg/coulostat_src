
#ifndef __MAIN_H__
#define __MAIN_H__


#ifdef __cplusplus
extern "C" {
#endif

typedef enum big_state_enum {
  DO_MENU
} big_state_enum_t;

// Yes, it's a big fat global.
extern big_state_enum_t app_state;


#ifdef __cplusplus
}
#endif

#endif
