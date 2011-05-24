
#ifndef __delay_h__
#define __delay_h__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void delay(unsigned long ms);
void delayMicroseconds(uint32_t us);

#ifdef __cplusplus
}
#endif

#endif
