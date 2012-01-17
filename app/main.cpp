// Interactive Test Session for LeafLabs Maple
// Copyright (c) 2010 LeafLabs LLC.
//
//  Useful for testing Maple features and troubleshooting. Select a COMM port
//  (SerialUSB or Serial2) before compiling and then enter 'h' at the prompt
//  for a list of commands.

#include "main.h"
#include "libmaple.h"
#include "wirish.h"

#include "my_systick.h"

#include "console.h"

#include "term_io.h"
#include "my_adc.h"
#include "buttons.h"
#include "sd_power.h"

#include "nokia_lcd/nokia6100.h"
#include "ui/gfx.h"
#include "ui/ui.h"
#include "ui/fonts.h"

#include "coulo_adc/coulo_adc.h"

#define LED_PIN 13
#define PWM_PIN  2

uint8 input = 0;
uint8 tiddle = 0;
int toggle = 0;
int rate = 0;
int sample = 0;


big_state_t current_app_state = DO_HALF_MENU;

// Not entirely happy with dedicating a variable to this.  Don't need to do 
// this if I take over the init function.
bool init_complete = false;

void setup() {

  init_adc();
  
  /* Set up the LED to blink  */
  pinMode(LED_PIN, OUTPUT);

  /* Start up the serial ports */
  Serial1.begin(115200);
  Serial1.println("Test test");

  console_init();
  console_splash();
  
  button_init();
  sd_init();

  nokia_init();
  nokia_reset();

  GfxInit();
  GfxClearScreen();
  GfxSetRect(0,0,132,132,FILL,BLACK);

  coulo_adc_init();
  
  init_systick();

  // For 16x8 font, should get 8 rows and 16 columns
  //for( row = 0; row < 8; row++ )
  //  for( col = 0; col < 16; col++ )
  //    LCDPutChar( 'a' + col, 16*row,8*col, LARGE,BLACK,YELLOW);
  //
  DEBUG.println("Finished with setup");
  init_complete = true;
}

void loop() {
  static uint8 addr = 0;
  static uint8_t bar_color = 0;

  toggle ^= 1;
  digitalWrite(LED_PIN, toggle);

  GfxSetPixel( 0,addr, bar_color ? RED : GREEN ); 
  addr++;
  if( addr > 131 ) { addr = 0; bar_color ^= 1; }

  delay(100);

  do_console();
}


// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated object that need libmaple may fail.
__attribute__(( constructor )) void premain() {
  init();
}

int main(void)
{
  unsigned char buttons;
  setup();

  while (1) {
    loop();

    buttons = button_get_keypress( BTN_ALL );
    button_press_debug( buttons );

    switch( current_app_state ) {
      case DO_HALF_MENU:
        draw_adc();
      case DO_FULL_MENU:
      case FILE_BROWSER:
        refresh_menu( buttons );
        break;
      case INFO_SCREEN:
        draw_info_screen( buttons );
        break;
    }

    internal_updated = false;
    reference2_adc_updated = false;

  }
  return 0;
}
