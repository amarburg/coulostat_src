
#ifndef __SD_SPI_STM32_H__
#define __SD_SPI_STM32_H__

#include "stdint.h"
#include "diskio.h"

#ifdef __cplusplus
extern "C" {
#endif

void disk_subsystem_init( void );

void disk_timerproc (void) __attribute__ ((long_call, section (".ramsection")));
void card_power(uint8_t on);
int chk_power(void);

DSTATUS disk_initialize ( byte_t dr );


#ifdef __cplusplus
}
#endif

#endif

