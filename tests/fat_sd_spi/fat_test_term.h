// FAT-testing-terminal, hacked from Martin Thomas's example
// "ChaN's FAT-Module with STM32 SPI"
// http://gandalf.arubi.uni-kl.de/avr_projects/arm_projects/arm_memcards/index.html
// copyright (c) 2011 Aaron Marburg

#ifndef FAT_TEST_TERM_H_
#define FAT_TEST_TERM_H_

#include "diskio.h"

#ifdef __cplusplus
extern "C" {
#endif

void fat_test_term_timerproc(void) __attribute__ ((section(".ramfunc")));
int fat_menu(void);

void put_dstatus( DSTATUS rc );

#ifdef __cplusplus
}
#endif

#endif

