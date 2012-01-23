
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

  /* Configure I/O for Flash Chip select */
  /*!!AMM investigate the 50MHz flag... */
  gpio_set_mode( MAX1303_CS_BASE, MAX1303_CS, GPIO_OUTPUT_PP );
  gpio_set_mode( MAX1303_SPI_BASE, MAX1303_SPI_SCK, GPIO_AF_OUTPUT_PP );
  gpio_set_mode( MAX1303_SPI_BASE, MAX1303_SPI_MISO, GPIO_INPUT_PD );
  gpio_set_mode( MAX1303_SPI_BASE, MAX1303_SPI_MOSI, GPIO_AF_OUTPUT_PP );

    /* De-select the Card: Chip Select high */
  MAX1303_DESELECT();
  
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

volatile uint8 tx_count;
volatile uint8 rx_count;
volatile uint8 max1303_data[16];
volatile bool completed_acq;

inline void start_conversion( uint8_t chan ) {
  spi_tx_reg( MAX1303_SPI, 0x80 | ( chan & MAX1303_CHAN_MASK)  );
}

// Had problems with IRQ basically firing continuously at PRESCALE_32
// Handle it with IRQs
void __irq_spi1( void )
{
  // First byte is sent manually, only need to send fifteen filler bytes.
  if( spi_is_tx_empty( MAX1303_SPI ) ) {
    if( tx_count < 16 )
      if( tx_count == 3 )
        start_conversion( MAX1303_CHAN1 );
      else if( tx_count == 7 )
        start_conversion( MAX1303_CHAN2 );
      else if( tx_count == 11 )
        start_conversion( MAX1303_CHAN3 );
      else
        spi_tx_reg( MAX1303_SPI, 0x00 );
    else
      spi_irq_disable( MAX1303_SPI, SPI_TXE_INTERRUPT );

    ++tx_count;
  }

  if( spi_is_rx_nonempty( MAX1303_SPI )) {
     //max1303_data |= spi_rx_reg( MAX1303_SPI );
     if( rx_count < 16 )
       max1303_data[rx_count] = spi_rx_reg( MAX1303_SPI ) & 0x000000FF;

     ++rx_count;
     if( rx_count >= 16 ) { completed_acq = true; }
  }

}

#define MAX1303_ACQ_TIMEOUT   100
int8_t max1303_acq_external_clock( uint16_t *data )
{
  uint16_t timeout = 0;
  int8_t retval = 0;
  uint8_t i;
  
  // Drain read register
  spi_rx_reg( MAX1303_SPI );

  // Initialize and arm the IRQ
  tx_count = 0;
  rx_count = 0;

  for( i = 0; i < 16; i++ ) { max1303_data[i] = 0; }

  completed_acq = false;

  MAX1303_SELECT();

  // Write first byte
  while( !spi_is_tx_empty( MAX1303_SPI )) { 
    delay_us(10);
    if( (timeout++) > MAX1303_ACQ_TIMEOUT ) { retval = -2; goto cleanup; } }
    timeout = 0;

    start_conversion( MAX1303_CHAN0 );

    spi_irq_enable( MAX1303_SPI, SPI_TXE_INTERRUPT | SPI_RXNE_INTERRUPT );

    while( !completed_acq ) {
    delay_us(1);
    if( (timeout++) > MAX1303_ACQ_TIMEOUT) {
      retval = -1; goto cleanup; 
    } 
  }

//  completed_acq = 255;

cleanup:
  spi_irq_disable( MAX1303_SPI, SPI_INTERRUPTS_ALL );
  MAX1303_DESELECT();

  while( spi_is_rx_nonempty( MAX1303_SPI ) ) { spi_rx_reg( MAX1303_SPI ); }
  while( !spi_is_tx_empty( MAX1303_SPI )) { ; }
  while( spi_is_busy( MAX1303_SPI)) {;}

  for( i = 0; i < 4; i++ ) {
    data[i] = max1303_data[4*i + 2]<<8 | max1303_data[4*i+3];
  }

  return retval;
}
