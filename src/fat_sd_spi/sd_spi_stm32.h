
#ifndef __SD_SPI_STM32_H__
#define __SD_SPI_STM32_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

void disk_timerproc (void) __attribute__ ((long_call, section (".ramsection")));
void card_power(uint8_t on);

#ifdef __cplusplus
}
#endif

#endif

