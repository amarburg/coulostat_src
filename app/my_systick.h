#ifndef _MY_SYSTICK_H_
#define _MY_SYSTICK_H_

#include "libmaple.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_systick( void );
uint32 get_systick_count( void );

#ifdef __cplusplus
}
#endif
#endif

