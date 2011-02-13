// Interactive Test Session for LeafLabs Maple
// Copyright (c) 2010 LeafLabs LLC.
//
//  Useful for testing Maple features and troubleshooting. Select a COMM port
//  (SerialUSB or Serial2) before compiling and then enter 'h' at the prompt
//  for a list of commands.

#include "wirish.h"

#include "my_systick.h"
#include "main.h"
#include "nokia6100.h"
#include "s1d15g00.h"

#include "term_io.h"

#include "sd_power.h"
#include "buttons.h"

#include "ui.h"
#include "fonts.h"

#define LED_PIN 13
#define PWM_PIN  2

uint8 input = 0;
uint8 tiddle = 0;
int toggle = 0;
int rate = 0;
int sample = 0;

// read these off maple board rev3
// note that 38 is just a button and 39+ aren't functional as of 04/22/2010
const uint8 pwm_pins[] = {0,1,2,3,5,6,7,8,9,11,12,14,24,25,27,28};
const uint8 adc_pins[] = {0,1,2,10,11,12,13,15,16,17,18,19,20,27,28};
#define NUM_GPIO        44      // 44 is the MAX
uint8 gpio_state[NUM_GPIO];

#define DUMMY_DAT "qwertyuiopasdfghjklzxcvbnmmmmmm,./1234567890-=qwertyuiopasdfghjklzxcvbnm,./1234567890"

void print_help(void);


big_state_enum_t app_state;


void setup() {
  //int row = 0, col=0;
  
  /* Set up the LED to blink  */
  pinMode(LED_PIN, OUTPUT);

  /* Start up the serial ports */
  Serial1.begin(115200);
  Serial1.println("Test test");
  //    Serial3.begin(9600);

  /* Send a message out over COMM interface */
  COMM.println(" ");
  COMM.println("    __   __             _      _");
  COMM.println("   |  \\/  | __ _ _ __ | | ___| |"); 
  COMM.println("   | |\\/| |/ _` | '_ \\| |/ _ \\ |");
  COMM.println("   | |  | | (_| | |_) | |  __/_|");
  COMM.println("   |_|  |_|\\__,_| .__/|_|\\___(_)");
  COMM.println("                 |_|");
  COMM.println("                              by leaflabs");
  COMM.println("");
  COMM.println("");
  COMM.println("Maple interactive test program (type '?' for help)");
  COMM.println("------------------------------------------------------------");
  COMM.print("> ");

  button_init();
  sd_init();

  nokia_init();
  nokia_reset();

  InitLcd();
  LCDClearScreen();
  LCDSetRect(0,0,132,132,FILL,BLACK);

  //LCDPutChar('1', 0,0, SS59PT, RED, BLACK );
  LCDPutStr( "123456", 4,0, SS30PT, RED, BLACK );
  LCDPutStr( "LARGE", 60,0, LARGE, YELLOW,BLACK );
  LCDPutStr( "MEDIUM", 80,0, MEDIUM, YELLOW,BLACK );
  LCDPutStr( "SMALL", 100,0, SMALL, YELLOW,BLACK );
  
  // For 16x8 font, should get 8 rows and 16 columns
  //for( row = 0; row < 8; row++ )
  //  for( col = 0; col < 16; col++ )
  //    LCDPutChar( 'a' + col, 16*row,8*col, LARGE,BLACK,YELLOW);
  //
  Serial1.println("Finished with setup");
}

void loop() {
  static uint8 addr = 0;
  static uint8_t bar_color = 0;

  toggle ^= 1;
  digitalWrite(LED_PIN, toggle);

  LCDSetPixel( 0,addr, bar_color ? RED : GREEN ); 
  addr++;
  if( addr > 131 ) { addr = 0; bar_color ^= 1; }

  delay(100);

  while(COMM.available()) {
    input = COMM.read();
    COMM.println(input);
    switch(input) {
      case 13:  // Carriage Return
        COMM.println("what?");
        break;
      case 32:  // ' '
        COMM.println("spacebar, nice!");
        break;
      case 63:  // '?'
      case 104: // 'h'
        print_help();
        break;
      case 117: // 'u'
        SerialUSB.println("Hello World!");
        break;
      case 119: // 'w'
        Serial1.println("Hello World!");
        Serial3.println("Hello World!");
        break;
      case 95:  // '_'
        COMM.println("Delaying for 5 seconds...");
        delay(5000);
        break;
      case 'p':
        COMM.println("Toggling SD card power ...");
        if( sd_chk_power() == true ) 
          COMM.println("SD power is on, turning off" );
        else
          COMM.println("SD power is off, turning on" );
        sd_toggle_power();
        /* Wait for settling */
        delay(100);
        if( sd_chk_power() == true ) 
          COMM.println("SD power is on");
        else
          COMM.println("SD power is off");
        break;
      case 114:  // 'r'
        COMM.println("Monitoring GPIO read state changes. Press enter.");
        // turn off LED
        digitalWrite(LED_PIN, 0);
        // make sure to skip the TX/RX headers
        for(int i = 2; i<NUM_GPIO; i++) {
          pinMode(i, INPUT_PULLDOWN);
          gpio_state[i] = (uint8)digitalRead(i);
        }
        while(!COMM.available()) { 
          for(int i = 2; i<NUM_GPIO; i++) {
            tiddle = (uint8)digitalRead(i);
            if(tiddle != gpio_state[i]) {
              COMM.print("State change on header D");
              COMM.print(i,DEC);
              if(tiddle) COMM.println(":\tHIGH");
              else COMM.println(":\tLOW");
              gpio_state[i] = tiddle;
            }
          }
        }
        for(int i = 2; i<NUM_GPIO; i++) {
          pinMode(i, OUTPUT);
        }
        break;
      default:
        COMM.print("Unexpected: ");
        COMM.println(input);
    }
    COMM.print("> ");
  }
}

void print_help(void) {
  COMM.println("");
  //COMM.println("Command Listing\t(# means any digit)");
  COMM.println("Command Listing");
  COMM.println("\t?: print this menu");
  COMM.println("\th: print this menu");
  COMM.println("\tw: print Hello World on all 3 USARTS");
}

static inline void key_press_debug( unsigned char keys )
{
  if( keys & BTN_UP ) Serial1.println("up");
  if( keys & BTN_DOWN ) Serial1.println("down");
  if( keys & BTN_LEFT ) Serial1.println("left");
  if( keys & BTN_RIGHT ) Serial1.println("right");
  if( keys & BTN_GREEN ) Serial1.println("green");
  if( keys & BTN_RED ) Serial1.println("red");
}




// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated object that need libmaple may fail.
__attribute__(( constructor )) void premain() {
  init();
}

int main(void)
{
  unsigned char keys;
  setup();

  while (1) {
    loop();

    keys = button_get_keypress( BTN_ALL );
    //Serial1.print(get_systick_count());
    //Serial1.print(' ');
    //Serial1.print(get_key_state());
    //Serial1.print(' ');
    //Serial1.println(keys+'a');

    key_press_debug( keys );

    switch( app_state ) {
      case DO_MENU:
        refresh_ui( keys );
        break;
    }
  }
  return 0;
}
