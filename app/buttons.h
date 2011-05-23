#ifndef __BUTTONS_H__
#define __BUTTONS_H__


// Map the six buttons as follows
#define BTN_LEFT 0x01
#define BTN_UP   0x02
#define BTN_DOWN 0x04
#define BTN_RIGHT 0x08
#define BTN_RED   0x10
#define BTN_GREEN 0x20

#define BTN_ALL   0x3F

#ifdef __cplusplus
extern "C" {
#endif

  void button_init( void );
void button_debounce_timerproc( void );
unsigned char button_get_keypress( unsigned char key_mask );

unsigned char get_key_state( void );
unsigned char get_key_pressed( void );

#ifdef __cplusplus
}
#endif

#endif
