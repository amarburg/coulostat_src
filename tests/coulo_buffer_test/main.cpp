// Interactive Test Session for LeafLabs Maple
// Copyright (c) 2010 LeafLabs LLC.
//
//  Useful for testing Maple features and troubleshooting. Select a COMM port
//  (SerialUSB or Serial2) before compiling and then enter 'h' at the prompt
//  for a list of commands.

#include "wirish.h"

#include "main.h"

#include "term_io.h"
#include "systick.h"
extern "C" void systick_attach_callback(void (*callback)(void));

#include "hw_config.h"
#include "coulo_adc/coulo_adc.h"
#include "coulo_adc/max1303.h"

uint8 input = 0;
uint8 toggle = 0;

typedef struct coulo_adc_record {
  uint16_t milliseconds;
  uint16_t adc_results[4];
} coulo_adc_record_t;

#define BUFFER_LENGTH   256
volatile uint32 buffer_count;
static struct coulo_adc_record buffer[BUFFER_LENGTH];

volatile uint32 systick_count = 0;
volatile static bool do_sample = false;
volatile static bool trigger_sample = false;

void my_systick(void)
{
  systick_count++;
  if( do_sample == true ) {
    buffer[buffer_count].milliseconds = systick_count;
    coulo_adc_read_nonblocking(  COULO_ADC_ALL, buffer[buffer_count].adc_results );
  }
}

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

  Serial1.println("Start coulo_adc_init");
  coulo_adc_init();

  systick_attach_callback( my_systick );
  Serial1.println("Finished with setup");
  init_complete = true;
}

void loop() {
  uint16_t i=0;

  toggle ^= 1;
  digitalWrite(LED_PIN, toggle);

  Serial1.println( systick_count );

  delay(100);

  while(COMM.available()) {
    input = COMM.read();
    switch(input) {
      case 13:  // Carriage Return
        COMM.println("what?");
        break;
      case 32:  // ' '
        COMM.println("spacebar, nice!");
        break;
      case 'a':
        COMM.println("Sampling ADC BUFFER_LENGTH times...");

        i = 0;
        buffer_count = 0;
        do_sample = true;

        while( !is_acq_completed() ) {
          toggle ^= 1;
          digitalWrite(LED_PIN, toggle);

          delay_us(10);
        }
        do_sample = false;

        /*COMM.println("Results");
        for( i = 0; i < 4; i++ ) {
          COMM.print(coulo_adc_results[i]);
          COMM.print(' ');
        }*/

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


