#ifndef _UI_INFO_H_
#define _UI_INFO_H_

#include "ui.h"


#ifdef __cplusplus
extern "C" {
#endif

bool info_callback( const struct menu_item *item );

void draw_info_screen( unsigned int keys );


#ifdef __cplusplus
}
#endif

#endif

