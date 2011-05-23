

#include <stdlib.h>
#include <stdio.h>

#include "my_adc.h"

#include "ui/adc.h"
#include "ui.h"

#include "term_io.h"

void draw_adc( void )
{
  float adc_value;
  char buf[10];

if( do_redraw == true ) {
    LCDSetRect(0,0,64,132,FILL,MENU_BG);

}

  if( internal_updated == true ) {
    adc_value = internal_v_adc * REFERENCE_SCALER;
    snprintf( buf,9, "%0.5f", adc_value );
    LCDPutStr( buf, 4,0, SS30PT, RED, BLACK );

    adc_value = internal_i_adc * REFERENCE_SCALER;
    snprintf( buf,9, "%0.5f", adc_value );
    LCDPutStr( buf, 34,0, SS30PT, RED, BLACK );

  }
  
}

