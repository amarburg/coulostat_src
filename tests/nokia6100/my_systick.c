
#include "libmaple.h"
#include "my_systick.h"
#include "main.h"

#include "buttons.h"
#include "my_adc.h"

volatile uint32 systick_count = 0;

//void SysTickHandler(void) __attribute__ ((long_call, section (".ramsection")));
void SysTickHandler(void) {
  if( init_complete == true ) {
    if( (systick_count % 50) == 0 ) button_debounce_timerproc();

    if( (systick_count % 250) == 0 ) {
      take_periodic_adc();
    }
  }

  systick_count++;
  if( systick_count > 999 ) systick_count = 0;
}

uint32 get_systick_count( void )
{
  return systick_count;
}

