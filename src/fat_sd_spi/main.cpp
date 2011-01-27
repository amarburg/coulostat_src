// interactive test session for leaflabs maple
// copyright (c) 2010 leaflabs llc.
//
//  useful for testing maple features and troubleshooting. select a comm port
//  (serialusb or serial2) before compiling and then enter 'h' at the prompt
//  for a list of commands.

#include "wirish.h"
#include "stdint.h"
#include "main.h"

#include "fat_test_term.h"
#include "sd_spi_stm32.h"

#define LED_PIN 13
#define PWM_PIN  2

uint8 input=0;
int toggle=0;

void print_help(void);
void SysTickHandler(void) __attribute__ ((long_call, section (".ramsection")));

/* use systick to call this every 1 ms 
* Function Name  : main_systick_action
* * Description    : operations to be done every 1ms
* * Input          : None
* * Output         : None
* * Return         : None
* *  overrides weak SysTick_Handler in startup_stm32*.c
* *  When a RAMFUNC every function called from inside the ISR must be
* *  reachable. This can be achieved by using compiler-option -mlong-calls
* *  which globally enables long-calls. Here this option has not been used
* *  instead the unreachable functions GPIO_Set/ResetBits have been replaced
* *  by direct register-writes and disk_timerproc has also been attributed
* *  as RAMFUNC to be reachable.
* *******************************************************************************/
void SysTickHandler(void) 
{
  /*static uint16_t cnt=0;
  static uint8_t flip=0;  */
  static uint8_t cntdiskio=0;

  //cnt++;
  //if( cnt >= 500 ) {
  //  cnt = 0;
    /* alive sign */
  //  if ( flip ) {
      // GPIO_SetBits(GPIO_LED, GPIO_Pin_LED2 );
      //                         GPIO_LED->BSRR = GPIO_Pin_LED2;
      //                                         } else {
      //                                                                 // GPIO_ResetBits(GPIO_LED, GPIO_Pin_LED2 );
      //                                                                                         GPIO_LED->BRR = GPIO_Pin_LED2;
      //                                                                                                         }
      //                                                                                                                         flip = !flip;
      //                                                                                                                                 }
      //
      //                                                                                                                                         cntdiskio++;
      if ( cntdiskio >= 10 ) {
        cntdiskio = 0;
        disk_timerproc(); /* to be called every 10ms */
      }

  fat_test_term_timerproc(); /* to be called every ms */
}

void setup() {
    /* Set up the LED to blink  */
    pinMode(LED_PIN, OUTPUT);

    /* Start up the serial ports */
    Serial1.begin(115200);
    Serial1.println("Test test");
    Serial3.begin(9600);

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
   return COMM.read();
}

int comm_test( void )
{
   return COMM.available();
}

void xputc( char c )
{
  COMM.write(c);
}



void loop() {
    toggle ^= 1;
    digitalWrite(LED_PIN, toggle);

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
            SerialUSB.println("Entering FAT test terminal");
            fat_test_term();
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
