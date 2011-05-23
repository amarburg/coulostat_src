

#ifndef _MENU_H_
#define _MENU_H_

typedef void (*second_line_callback_t)( char *buf, uint8 buflen )

typedef struct menu_entry_t {
  const char *name;
  second_line_callback_t second_line_callback;
}

  

#endif
