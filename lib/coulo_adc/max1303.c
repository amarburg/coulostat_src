
#include <stdbool.h>

#include "libmaple.h"
#include "spi.h"
#include "gpio.h"
#include "rcc.h"
#include "timer.h"

#include "hw_config.h"
#include "max1303.h"

/* - for SPI1 and full-speed APB2: 72MHz/4 */
#define MAX1303_SPI_PRESCALER  SPI_BAUD_PCLK_DIV_64


static uint8_t spi_xmit( uint8_t val )
{
  uint8 retval = 0;

  // Follows the procedure from the reference manual RM0008 pp 665
  //while( !spi_is_tx_empty( MAX1303_SPI )) { ; }
  spi_tx_reg( MAX1303_SPI, val );

  while( !spi_is_rx_nonempty( MAX1303_SPI ) ) {;}
  retval = spi_rx_reg( MAX1303_SPI ) & 0x00FF;

  while( !spi_is_tx_empty( MAX1303_SPI )) { ; }
  while( spi_is_busy( MAX1303_SPI)) {;}
}

void max1303_init( void )
{
  spi_peripheral_disable( MAX1303_SPI );

  /* De-select the Card: Chip Select high */
  MAX1303_DESELECT();
 
  /* Configure I/O for Flash Chip select */
  /*!!AMM investigate the 50MHz flag... */
  gpio_set_mode( MAX1303_CS_BASE, MAX1303_CS, GPIO_OUTPUT_PP );
  gpio_set_mode( MAX1303_SPI_BASE, MAX1303_SPI_SCK, GPIO_AF_OUTPUT_PP );
  gpio_set_mode( MAX1303_SPI_BASE, MAX1303_SPI_MISO, GPIO_INPUT_PD );
  gpio_set_mode( MAX1303_SPI_BASE, MAX1303_SPI_MOSI, GPIO_AF_OUTPUT_PP );

  timer_set_mode(TIMER3, 2, TIMER_DISABLED);
  timer_set_mode(TIMER3, 1, TIMER_DISABLED);

  spi_init( MAX1303_SPI );

  spi_irq_disable( MAX1303_SPI, SPI_INTERRUPTS_ALL );
  spi_tx_dma_disable( MAX1303_SPI );
  spi_rx_dma_disable( MAX1303_SPI );

  // Mode = 0x00 means CPOL = 0, CPHA = 0
  spi_master_enable( MAX1303_SPI, MAX1303_SPI_PRESCALER, 
                     SPI_MODE_0, SPI_FRAME_MSB | SPI_DFF_8_BIT );

  /* drain SPI */
//  spi_rx_reg( MAX1303_SPI );
//  spi_tx_reg( MAX1303_SPI, 0x00 );
}

void max1303_mode_config( unsigned char mode )
{
  unsigned char b = ((mode & 0x07) << 4) | 0x88;
  MAX1303_SELECT();
  spi_xmit( b );
  MAX1303_DESELECT();
}

void max1303_analog_input_config( unsigned char chan, unsigned char config )
{
  unsigned char b = (0x80 | (chan & MAX1303_CHAN_MASK) | (config & MAX1303_RANGE_MASK) );
  MAX1303_SELECT();
  spi_xmit( b );
  MAX1303_DESELECT();
}

void max1303_partial_power_down( void ) { max1303_mode_config( MODE_PARTIAL_POWER_DOWN ); };
void max1303_full_power_down( void )    { max1303_mode_config( MODE_FULL_POWER_DOWN ); };
void max1303_wake( void )               { max1303_mode_config( MAX1303_MODE ); };


// Under external clock mode, you run SCLK for 4 bytes (32 clocks)
// For the first byte you write the conversion initiation byte
// For the second byte, no read or write
// For the third and fourth bytes you clock 16 bits back in

volatile uint8 channel;
volatile uint8 channel_mask;

volatile uint8 tx_count;
volatile uint8 rx_count;
volatile uint16 *max1303_data;
volatile bool completed_acq;
volatile bool tx_complete;

inline void start_conversion( uint8_t chan ) {
  spi_tx_reg( MAX1303_SPI, 0x80 | ( (chan<<CHANNEL_SHIFT) & MAX1303_CHAN_MASK)  );
}

// Had problems with IRQ basically firing continuously at PRESCALE_32
void __irq_spi1( void )
{
  register uint8_t rx;

  if( spi_is_tx_empty( MAX1303_SPI ) && channel < 4 )  {
    if( (tx_count & 0x03) == 0 ) {
      while( (((0x01 << ++channel) & channel_mask) == 0) && (channel <= 3) ) { ; }

      if( channel > 3 ) {
        spi_irq_disable( MAX1303_SPI, SPI_TXE_INTERRUPT );
      } else {
        start_conversion( channel );
        ++tx_count;
      }
    } else {
      spi_tx_reg( MAX1303_SPI, 0x00 );
      ++tx_count;
    }
  }

  if( spi_is_rx_nonempty( MAX1303_SPI )) {
    rx = spi_rx_reg( MAX1303_SPI ) & 0x000000FF;

    if ( rx_count & 0x02 ) {
      if( rx_count & 0x01 )
        max1303_data[ (rx_count & 0xFC) >> 2 ] |= rx; 
      else
        max1303_data[ (rx_count & 0xFC) >> 2 ] = rx << 8;
    } 
    ++rx_count;

    if (rx_count >= tx_count)  {
      completed_acq = true;
      spi_irq_disable( MAX1303_SPI, SPI_INTERRUPTS_ALL );
      MAX1303_DESELECT();
    }
  }
}

bool is_acq_completed( void ) { return completed_acq; }

static void max1303_cleanup( void )
{
  spi_irq_disable( MAX1303_SPI, SPI_INTERRUPTS_ALL );
  MAX1303_DESELECT();

  while( spi_is_rx_nonempty( MAX1303_SPI ) ) { spi_rx_reg( MAX1303_SPI ); }
  while( !spi_is_tx_empty( MAX1303_SPI )) { ; }
  while( spi_is_busy( MAX1303_SPI)) {;}
}


#define MAX1303_ACQ_TIMEOUT   100
int8_t max1303_acq_external_clock_nonblocking( uint8_t chans,  uint16_t *data )
{
  uint16_t timeout = 0;
  int8_t retval = 0;
  
  // Drain read register
  spi_rx_reg( MAX1303_SPI );

  // Initialize and arm the IRQ
  tx_count = 1;
  rx_count = 0;
  tx_complete = false;
  completed_acq = false;
  channel_mask = chans;
  channel = 0;
  max1303_data = data;

  while( ((0x01 << channel) & chans) == 0 ) { if( ++channel > 3 ) return 0; }

  MAX1303_SELECT();

  while( !spi_is_tx_empty( MAX1303_SPI )) { 
    delay_us(1);
    if( (timeout++) > MAX1303_ACQ_TIMEOUT ) { retval = -2; goto cleanup; } 
  }
  start_conversion( channel );
  spi_irq_enable( MAX1303_SPI, SPI_TXE_INTERRUPT | SPI_RXNE_INTERRUPT );


  return 0;

cleanup:
  max1303_cleanup();
  return retval;
}

int8_t max1303_acq_external_clock_blocking( uint8_t chans,  uint16_t *data )
{
   int retval = max1303_acq_external_clock_nonblocking( chans, data );
   if( retval < 0 ) return retval;

   while( ! is_acq_completed() ) {
     delay_us(10);
   }

   max1303_cleanup();

   return 0;
}
