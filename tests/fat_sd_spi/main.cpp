// interactive test session for leaflabs maple
// copyright (c) 2010 leaflabs llc.
//
//  useful for testing maple features and troubleshooting. select a comm port
//  (serialusb or serial2) before compiling and then enter 'h' at the prompt
//  for a list of commands.

#include "wirish.h"
#include "stdint.h"
#include "main.h"

#include "hw_config.h"

#include "fat_test_term.h"
#include "sd_spi_stm32.h"

extern "C" void systick_attach_callback(void (*callback)(void));

void my_systick( void )
{
  static uint16_t count = 0;
  static int toggle = 0;

  if( (count % 10) == 0 ) {
    disk_timerproc();
  }

  if( (count % 100) == 0 ) {
    toggle ^= 1;
  //  digitalWrite(LED_PIN, toggle);
  }

  count++;
  if( count >= 1000 ) count = 0;

  fat_test_term_timerproc(); /* to be called every ms */
}


uint8 input=0;

void print_help(void);

enum app_state_t {
  DO_MAIN_MENU,
  DO_FAT_MENU
} app_state = DO_MAIN_MENU;

void return_to_main_menu( void ) { app_state = DO_MAIN_MENU; }

void setup() {
    /* Set up the LED to blink  */
    pinMode(LED_PIN, OUTPUT);
    debug_led(0);

    /* Start up the serial ports */
    Serial1.begin(115200);
    Serial1.println("Test test");
    /*Serial3.begin(9600); */
    
    systick_attach_callback( my_systick );

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


    disk_subsystem_init();
}

// Compatibility wrappers with Martin Thomas' term_io functions for now
void comm_println( const char *c )
{
  COMM.println(c);
}

void comm_put( char c )
{
  COMM.write(c);
}

char comm_get( void )
{
  uint8 b = COMM.read();
  Serial1.print( b );
   return b;
}

int comm_test( void )
{
   return COMM.available();
}

void xputc( char c )
{
  COMM.write(c);
}


void debug_println( const char *c )
{
  DEBUG.println(c);
}


void debug_led( unsigned int i )
{
  digitalWrite(LED_PIN, i );
}

void loop() {

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
          case 'p':
            card_power( 1 );
            break;
          case 'P':
            card_power( 0 );
            break;
          case 'f':
            COMM.println("Entering FAT test terminal");
            app_state = DO_FAT_MENU;
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
  COMM.println("\tu: print Hello World on USB");
  COMM.println("\tf: FAT test menu");
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
    switch( app_state ) {
      case DO_MAIN_MENU:
        loop();
        break;
      case DO_FAT_MENU:
        fat_menu();
        break;
    }
  }
  return 0;
}
