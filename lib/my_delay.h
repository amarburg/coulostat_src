
#ifndef __delay_h__
#define __delay_h__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  // These functions are lifted straight from the wirish_time.[hc] 
  // files in libmaple.  I just wanted them to be in the C namespace

void delay(unsigned long ms);
void delayMicroseconds(uint32_t us);

#ifdef __cplusplus
}
#endif

#endif
