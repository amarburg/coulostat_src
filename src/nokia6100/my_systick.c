
#include "libmaple.h"
#include "my_systick.h"

#include "buttons.h"
#include "my_adc.h"

volatile uint32 systick_count = 0;

//void SysTickHandler(void) __attribute__ ((long_call, section (".ramsection")));
void SysTickHandler(void) {
  if( (systick_count % 50) == 0 ) button_debounce_timerproc();

  if( (systick_count % 250) == 0 ) {
    reference2_adc++;
    reference2_adc_updated = true;
  }

  systick_count++;
  if( systick_count > 999 ) systick_count = 0;
}

uint32 get_systick_count( void )
{
  return systick_count;
}

