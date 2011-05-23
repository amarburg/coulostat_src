
#include <libmaple.h>
#include "fat_test_term.h"
#include "sd_spi_stm32.h"
#include "main.h"

__attribute__ ((long_call, section (".ramsection"))) void SysTickHandler(void) 
{
  static uint16_t count = 0;
  static int toggle = 0;

  if( (count % 10) == 0 ) {
    disk_timerproc();
  }
  if( (count % 100) == 0 ) {
    toggle ^= 1;
    digitalWrite(LED_PIN, toggle);
  }
  count++;
  if( count >= 1000 ) count = 0;

  fat_test_term_timerproc(); /* to be called every ms */
  //
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
}


