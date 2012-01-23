// Interactive Test Session for LeafLabs Maple
// Copyright (c) 2010 LeafLabs LLC.
//
//  Useful for testing Maple features and troubleshooting. Select a COMM port
//  (SerialUSB or Serial2) before compiling and then enter 'h' at the prompt
//  for a list of commands.

#include "wirish.h"

#include "main.h"

#include "term_io.h"
#include "hw_config.h"
#include "coulo_adc/coulo_adc.h"

#define PWM_PIN  2

uint8 input = 0;
uint8 tiddle = 0;
uint8 toggle = 0;
uint8 loop_count = 0;

// Not entirely happy with dedicating a variable to this.  Don't need to do 
// this if I take over the init function.
bool init_complete = false;

void setup() {
 
  /* Set up the LED to blink  */
  pinMode(LED_PIN, OUTPUT);

  /* Start up the serial ports */
  Serial1.begin(115200);
  Serial1.println("Test test");

  Serial1.println("Start coulo_adc_init");
  coulo_adc_init();

  Serial1.println("Finished with setup");
  init_complete = true;
}

void loop() {
  static uint8_t channel = 0x01;
  uint16_t coulo_adc_results[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
  int8_t retval;
  uint8_t i = 0,j = 0;
  toggle ^= 1;
  digitalWrite(LED_PIN, toggle);

  Serial1.println( loop_count++ );

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
      case 'a':
        channel = COULO_ADC_ALL;
      case 'A':
        COMM.print("Sampling ADCs: ");
        if( channel & COULO_ADC_0 ) COMM.print("0 ");
        if( channel & COULO_ADC_1 ) COMM.print("1 ");
        if( channel & COULO_ADC_2 ) COMM.print("2 ");
        if( channel & COULO_ADC_3 ) COMM.print("3 ");
        COMM.println();

        //retval = coulo_adc_read_blocking( COULO_ADC_ALL, coulo_adc_results );

        // Cycle through all of the ADC channels
        retval = coulo_adc_oversample_blocking( channel, 3, coulo_adc_results );
        if( ++channel > COULO_ADC_ALL ) { channel = COULO_ADC_0; }
        
        if( retval != 0 ) {
          COMM.print("Error: ");
          COMM.print(retval);
          COMM.println();
        }

        COMM.println("Results");
        for( j = 0; j < 3; j++ ) {
        for( i = 0; i < 4; i++ ) {
          COMM.print(coulo_adc_results[4*j+i]);
          COMM.print(' ');
        }
        COMM.println();
        }

        COMM.println("");
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
  setup();

  while (1) {
    loop();
  }
  return 0;
}
