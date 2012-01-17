// Interactive Test Session for LeafLabs Maple
// Copyright (c) 2010 LeafLabs LLC.
//
//  Useful for testing Maple features and troubleshooting. Select a COMM port
//  (SerialUSB or Serial2) before compiling and then enter 'h' at the prompt
//  for a list of commands.

#include "wirish.h"

#include "main.h"

#include "term_io.h"

#include "max1303/coulo_adc.h"

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


big_state_t current_app_state = DO_HALF_MENU;

// Not entirely happy with dedicating a variable to this.  Don't need to do 
// this if I take over the init function.
bool init_complete = false;

void setup() {
 
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

  Serial1.println("Start coulo_adc_init");
  coulo_adc_init();

  // For 16x8 font, should get 8 rows and 16 columns
  //for( row = 0; row < 8; row++ )
  //  for( col = 0; col < 16; col++ )
  //    LCDPutChar( 'a' + col, 16*row,8*col, LARGE,BLACK,YELLOW);
  //
  Serial1.println("Finished with setup");
  init_complete = true;
}

void loop() {
  toggle ^= 1;
  digitalWrite(LED_PIN, toggle);
  Serial1.println("Loop!");

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
      case 'a':
        COMM.println("Sampling ADC...");
        uint16_t coulo_adc_results[4];
        coulo_adc_read( COULO_ADC_0, coulo_adc_results );

        COMM.println("Results");
        COMM.print(coulo_adc_results[0]);
        COMM.print(coulo_adc_results[1]);
        COMM.print(coulo_adc_results[2]);
        COMM.print(coulo_adc_results[3]);
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
  unsigned char keys;
  setup();

  while (1) {
    loop();
  }
  return 0;
}
