
#include "libmaple.h"
#include "spi.h"
#include "gpio.h"
#include "rcc.h"
#include "timer.h"

#include "max1303.h"

/* - for SPI1 and full-speed APB2: 72MHz/4 */
#define MAX1303_SPI_PRESCALER  SPI_BAUD_PCLK_DIV_32


static uint8_t spi_xmit( uint8_t val )
{
  while( !spi_is_tx_empty( MAX1303_SPI )) { ; }
  spi_tx_reg( MAX1303_SPI, val );

  while( !spi_is_rx_nonempty( MAX1303_SPI ) ) {;}
  return spi_rx_reg( MAX1303_SPI ) & 0x00FF;
}

void max1303_init( void )
{
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

  // This handles GPIO setup on SCK, MISO and MOSI
  // Mode = 0x00 means CPOL = 0, CPHA = 0
  spi_init( MAX1303_SPI );

  spi_irq_disable( MAX1303_SPI, SPI_INTERRUPTS_ALL );
  spi_tx_dma_disable( MAX1303_SPI );
  spi_rx_dma_disable( MAX1303_SPI );



  spi_master_enable( MAX1303_SPI, MAX1303_SPI_PRESCALER, 
                     SPI_MODE_0, SPI_FRAME_MSB | SPI_DFF_8_BIT | SPI_SW_SLAVE | SPI_SOFT_SS );

  /* drain SPI */
  spi_rx_reg( MAX1303_SPI );
}



#define MAX1303_MODE_MASK 0xF8
void max1303_mode_config( unsigned char mode )
{
  unsigned char b = (0x88 | mode) & MAX1303_MODE_MASK;
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
volatile uint16 max1303_data;
volatile uint8 completed_acq;

// Handle it with IRQs
void __irq_spi1( void )
{
  if( spi_is_tx_empty( MAX1303_SPI ) && tx_count < 4 ) {
    spi_tx_reg( MAX1303, 0x00 );
    ++tx_count;
  }

  if( spi_is_rx_nonempty( MAX1303_SPI )) {
     data |= spi_rx_reg( MAX1303_SPI );
     if( rx_count == 0 ) data <<= 8;
     rx_count++;

     if( rx_count == 2 ) completed_acq = 1;
  }

}

uint16_t max1303_acq_external_clock( unsigned char chan )
{
  uint16_t data = 0xAAAA;

  MAX1303_SELECT();

  // Drain read register
  spi_rx_reg( MAX1303_SPI );

  // Write first byte
  while( !spi_is_tx_empty( MAX1303_SPI )) { ; }
  spi_tx_reg( MAX1303_SPI, 0x80 | (chan & MAX1303_CHAN_MASK)  );

  // Write second byte, read first reply (0)
  while( !spi_is_tx_empty( MAX1303_SPI )) { ; }
  spi_xmit( 0x0 );
  while( !spi_is_rx_nonempty( MAX1303_SPI ) ) {;}
  spi_rx_reg(MAX1303_SPI );

  // Write third byte, read second reply (0)
  while( !spi_is_tx_empty( MAX1303_SPI )) { ; }
  spi_xmit( 0x0 );
  while( !spi_is_rx_nonempty( MAX1303_SPI ) ) {;}
  data |= spi_rx_reg(MAX1303_SPI );

  // Write fourth byte, read third reply (high byte)
  while( !spi_is_tx_empty( MAX1303_SPI )) {  ; }
  spi_xmit( 0x0 );
  while( !spi_is_rx_nonempty( MAX1303_SPI ) ) {;}
  data = (spi_rx_reg(MAX1303_SPI)) << 8;

  // Read fourth reply (low byte)
  while( !spi_is_rx_nonempty( MAX1303_SPI ) ) {;}
  data |= spi_rx_reg(MAX1303_SPI );
  MAX1303_DESELECT();

  return data;
}
