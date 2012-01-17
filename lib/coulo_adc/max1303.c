
#include "libmaple.h"
#include "spi.h"
#include "gpio.h"
#include "rcc.h"
#include "timer.h"

#include "max1303.h"

/* - for SPI1 and full-speed APB2: 72MHz/4 */
#define MAX1303_SPI_PRESCALER  SPI_BAUD_PCLK_DIV_8

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;

#define RCC_APB2Periph_SPI1              ((uint32_t)0x00001000)

#define CR2_TXEIE     0x00000080
#define CR2_RXNEIE    0x00000040
#define CR2_ERRIE     0x00000020
#define CR2_TXDMAEN   0x00000002
#define CR2_RXDMAEN   0x00000001


static uint8_t spi_xmit( uint8_t val )
{
  while( !spi_is_tx_empty( MAX1303_SPI )) { ; }
  spi_tx_reg( MAX1303_SPI, val );

  while( !spi_is_rx_reg_nonempty( MAX1303_SPI ) ) {;}
  return spi_rx_ref( MAX1303_SPI ) & 0x00FF;
}

void max1303_init( void )
{
  volatile unsigned char dummyread;

  /* Configure I/O for Flash Chip select */
  /*!!AMM investigate the 50MHz flag... */
  gpio_set_mode( MAX1303_CS_BASE, MAX1303_CS, GPIO_OUTPUT_PP );
  // Other pins are handled by spi_init(...)

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
  spi_master_enable( MAX1303_SPI, MAX1303_SPI_PRESCALER, SPI_MODE_0,
      SPI_FRAME_MSB );

  /* drain SPI */
//  dummyread = spi_rx( MAX1303_SPI );
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
uint16_t max1303_acq_external_clock( unsigned char chan )
{
  uint16_t data = 0;

  // libmaple's spi_xmit is bi-directional, so it does:
  //    1. wait for TXE
  //    2. Push to spi->DR
  //    3. wait for RXNE
  //    4. Pull spi->DR

  MAX1303_SELECT();
  spi_xmit( 0x80 | (chan & MAX1303_CHAN_MASK)  );
  spi_xmit( 0x0 );
  data = spi_xmit( 0x0 ) << 8; 
  data |= spi_xmit( 0x0 );
  MAX1303_DESELECT();

  return data;
}
