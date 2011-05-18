

#ifndef __SD_POWER_H__
#define __SD_POWER_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void sd_init( void );
void sd_power( bool on );
void sd_toggle_power( void );
bool sd_chk_power( void );

#ifdef __cplusplus
}
#endif

#endif
