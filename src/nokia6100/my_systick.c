
#include "libmaple.h"
#include "my_systick.h"

#include "buttons.h"

volatile uint32 systick_count = 0;

//void SysTickHandler(void) __attribute__ ((long_call, section (".ramsection")));
void SysTickHandler(void) {
  if( (systick_count % 50) == 0 ) button_debounce_timerproc();

  systick_count++;
  if( systick_count > 999 ) systick_count = 0;
}

uint32 get_systick_count( void )
{
  return systick_count;
}

