

#include <stdlib.h>
#include <stdio.h>

#include "my_adc.h"

#include "ui/adc.h"
#include "ui.h"


void draw_adc( void )
{
  float adc_value;
  char buf[10];

  if( reference2_adc_updated == true ) {
    adc_value = reference2_adc * REFERENCE_SCALER;
    snprintf( buf,9, "% .5f", adc_value );

    LCDPutStr( buf, 4,0, SS30PT, RED, BLACK );
  }
  
}

