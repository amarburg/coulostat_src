// Interactive Test Session for LeafLabs Maple
// Copyright (c) 2010 LeafLabs LLC.
//
//  Useful for testing Maple features and troubleshooting. Select a COMM port
//  (SerialUSB or Serial2) before compiling and then enter 'h' at the prompt
//  for a list of commands.

#include "wirish.h"

#include "nokia6100.h"
#include "s1d15g00.h"

#include "sd_power.h"

#define LED_PIN 13
#define PWM_PIN  2

// choose your weapon
#define COMM SerialUSB
//#define COMM Serial2

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
void do_noise(uint8 pin);
void do_everything(void);
void do_fast_gpio(void);


char key_state;
char key_pressed;


// Map the six buttons as follows
#define BTN_UP   0x01
#define BTN_DOWN 0x02
#define BTN_LEFT 0x04
#define BTN_RIGHT 0x08
#define BTN_GREEN 0x10
#define BTN_RED   0x20
#define BTN_ALL   0x3F

#define BTN_GPIO   GPIOC_BASE
#define BTN_UP_PIN 0
#define BTN_DOWN_PIN 1
#define BTN_LEFT_PIN 2
#define BTN_RIGHT_PIN 3
#define BTN_GREEN_PIN 4
#define BTN_RED_PIN 5


void button_init( void )
{
gpio_set_mode( BTN_GPIO, BTN_UP_PIN, GPIO_MODE_INPUT_FLOATING );
gpio_set_mode( BTN_GPIO, BTN_DOWN_PIN, GPIO_MODE_INPUT_FLOATING );
gpio_set_mode( BTN_GPIO, BTN_LEFT_PIN, GPIO_MODE_INPUT_FLOATING );
gpio_set_mode( BTN_GPIO, BTN_RIGHT_PIN, GPIO_MODE_INPUT_FLOATING );
gpio_set_mode( BTN_GPIO, BTN_GREEN_PIN, GPIO_MODE_INPUT_FLOATING );
gpio_set_mode( BTN_GPIO, BTN_RED_PIN, GPIO_MODE_INPUT_FLOATING );
}

/* Use a version of the Danni debounce from here:
 * http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=189356
 * As noted, this requires 4 consecutive samples to trigger
 */
void button_debounce_timerproc( void )  __attribute__ ((long_call, section (".ramsection")));

void button_debounce_timerproc( void )
{
  static char ct0, ct1; 
  char i; 
  // Assemble key_input from GPIOs (inefficient for now)
  char key_input =( gpio_read_bit( BTN_GPIO, BTN_UP_PIN ) ? BTN_UP : 0 ) | 
                  ( gpio_read_bit( BTN_GPIO, BTN_DOWN_PIN ) ? BTN_DOWN : 0 ) |
                  ( gpio_read_bit( BTN_GPIO, BTN_LEFT_PIN ) ? BTN_LEFT : 0 ) |
                  ( gpio_read_bit( BTN_GPIO, BTN_RIGHT_PIN ) ? BTN_RIGHT : 0 ) |
                  ( gpio_read_bit( BTN_GPIO, BTN_GREEN_PIN ) ? BTN_GREEN : 0 ) |
                  ( gpio_read_bit( BTN_GPIO, BTN_RED_PIN ) ? BTN_RED : 0 ); 

  i = key_state ^ ~key_input;           // key changed ? 
  ct0 = ~( ct0 & i );                   // reset or count ct0 
  ct1 = ct0 ^ (ct1 & i);                // reset or count ct1 
  i &= ct0 & ct1;                       // count until roll over ? 
  key_state ^= i;                       // then toggle debounced state 
  // now debouncing finished 
  key_pressed |= key_state & i;           // 0->1: key press detect 
} 

char button_get_keypress( char key_mask ) 
{ 
  // Do I want to do this?
  //nvic_irq_disable_all();
  key_mask &= key_pressed;                // read key(s) 
  key_pressed ^= key_mask;                // clear key(s) 
  //nvic_irq_enable_all();
  return key_mask; 
} 

//void SysTickHandler(void) __attribute__ ((long_call, section (".ramsection")));
//void SysTickHandler(void)
//{
//  static unsigned int count = 0;

  /* Call the debounce code at 10 Hz */
  //if( (count % 100) == 0 ) button_debounce_timerproc();

// if( ++count > 999 ) count = 0;
//}

void setup() {
  int row = 0;
  int col = 0;
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

    sd_init();

    nokia_init();
    nokia_reset();

    InitLcd();
    LCDClearScreen();
    LCDSetRect(0,0,132,132,FILL,BLACK);

    // For 16x8 font, should get 8 rows and 16 columns
    for( row = 0; row < 8; row++ )
      for( col = 0; col < 16; col++ )
        LCDPutChar( 'a' + col, 16*row,8*col, LARGE,BLACK,YELLOW);
}

void loop() {
  static uint8 addr = 0;

    toggle ^= 1;
    digitalWrite(LED_PIN, toggle);

    LCDSetPixel( 20,addr, RED ); 
    addr++;
    if( addr > 131 ) { addr = 0; }

    LCDPutStr("HELP!", 80,20, SMALL, BLACK, YELLOW);

    LCDSetRect( 90, 70, 75, 120, FILL, YELLOW );

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


// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated object that need libmaple may fail.
 __attribute__(( constructor )) void premain() {
    init();
}

int main(void)
{
  char keys;
    setup();

    while (1) {
        loop();

        keys = button_get_keypress( BTN_ALL );
        if( keys & BTN_UP )
          COMM.println("Up!");
        if( keys & BTN_DOWN )
          COMM.println("Down!");
        if( keys & BTN_LEFT )
          COMM.println("Left!");
        if( keys & BTN_RIGHT )
          COMM.println("Right!");
        if( keys & BTN_GREEN )
          COMM.println("Green!");
        if( keys & BTN_RED )
          COMM.println("Red!");

    }
    return 0;
}
