
#include "max1303.h"
#include "coulo_adc.h"

void coulo_adc_init( void )
{
  max1303_init();

  // Configure MAX1303
  max1303_mode_config( MODE_RESET );
  max1303_analog_input_config( MAX1303_CHAN0, SE_PLUS_MINUS_VREF );
  max1303_analog_input_config( MAX1303_CHAN1, SE_PLUS_MINUS_VREF );
  max1303_analog_input_config( MAX1303_CHAN2, SE_PLUS_MINUS_VREF );
  max1303_analog_input_config( MAX1303_CHAN3, SE_PLUS_MINUS_VREF );
  max1303_mode_config( MAX1303_MODE );
}

// Hm, embarrassing?
inline unsigned char chan_to_chan( unsigned char b )
{
  return ((b & 0x03) << CHANNEL_SHIFT); 
}

// As a start, just do it all inline
int8_t coulo_adc_read_blocking( uint8_t chans, uint16_t *results )
{
  return max1303_acq_external_clock_blocking( chans, results );
}

int8_t coulo_adc_read_nonblocking( uint8_t chans, uint16_t *results )
{
  return max1303_acq_external_clock_nonblocking( chans, results );
}
