

#include <stdlib.h>
#include <stdio.h>

#include "my_adc.h"

#include "ui/adc.h"
#include "ui/gfx.h"
#include "ui.h"

#include "term_io.h"

void draw_adc( void )
{
  float adc_value;
  char buf[10];

  if( do_redraw == true ) {
    GfxSetRect(0,0,64,132,FILL,MENU_BG);

  }

  if( internal_updated == true ) {
    adc_value = (internal_v_adc-2048) * INTERNAL_ADC_SCALER;
    snprintf( buf,9, "% 0.3f", adc_value );
    GfxPutStr( buf, 4,0, SS30PT, RED, BLACK );

    adc_value = (internal_i_adc-2048) * INTERNAL_ADC_SCALER;
    snprintf( buf,9, "% 0.3f", adc_value );
    GfxPutStr( buf, 34,0, SS30PT, RED, BLACK );

  }
  
}

