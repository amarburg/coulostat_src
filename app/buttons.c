
#include "libmaple.h"
#include "gpio.h"
#include "buttons.h"
#include "main.h"


volatile unsigned char key_state;
volatile unsigned char key_pressed;

#define BTN_GPIO   GPIOC

#define BTN_LEFT_PIN   0
#define BTN_UP_PIN     1
#define BTN_DOWN_PIN   2
#define BTN_RIGHT_PIN  3
#define BTN_RED_PIN    4
#define BTN_GREEN_PIN  5


void button_init( void )
{

  gpio_set_mode( BTN_GPIO, BTN_UP_PIN, GPIO_INPUT_FLOATING );
  gpio_set_mode( BTN_GPIO, BTN_DOWN_PIN, GPIO_INPUT_FLOATING );
  gpio_set_mode( BTN_GPIO, BTN_LEFT_PIN, GPIO_INPUT_FLOATING );
  gpio_set_mode( BTN_GPIO, BTN_RIGHT_PIN, GPIO_INPUT_FLOATING );
  gpio_set_mode( BTN_GPIO, BTN_GREEN_PIN, GPIO_INPUT_FLOATING );
  gpio_set_mode( BTN_GPIO, BTN_RED_PIN, GPIO_INPUT_FLOATING );
}

/* Use a version of the Danni debounce from here:
 * http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=189356
 * As noted, this requires 4 consecutive samples to trigger
 */
//void button_debounce_timerproc( void )  __attribute__ ((long_call, section (".ramsection")));
void button_debounce_timerproc( void )
{
  static unsigned char ct0=0, ct1=0; 
  unsigned char i; 
  unsigned  char key_input;
  
  // Assemble key_input from GPIOs (inefficient for now)
  key_input =( gpio_read_bit( BTN_GPIO, BTN_UP_PIN ) ? BTN_UP : 0 ) | 
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

unsigned char button_get_keypress( unsigned char key_mask ) 
{ 
  // Do I want to do this?
  //nvic_irq_disable_all();
  key_mask &= key_pressed;                // read key(s) 
  key_pressed ^= key_mask;                // clear key(s) 
  
  //nvic_irq_enable_all();
  return key_mask; 
} 

unsigned char get_key_state( void ) 
{
  return key_state;
}
unsigned char get_key_pressed( void ) 
{
  return key_pressed;
}

void button_press_debug( unsigned char keys )
{
  if( keys & BTN_UP ) debug_println("up");
  if( keys & BTN_DOWN ) debug_println("down");
  if( keys & BTN_LEFT ) debug_println("left");
  if( keys & BTN_RIGHT ) debug_println("right");
  if( keys & BTN_GREEN ) debug_println("green");
  if( keys & BTN_RED ) debug_println("red");
}


