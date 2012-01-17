
#include "libmaple.h"
#include "spi.h"
#include "gpio.h"
#include "rcc.h"
#include "timers.h"

#include "max1303.h"

/* - for SPI1 and full-speed APB2: 72MHz/4 */
#define MAX1303_SPI_PRESCALER  CR1_BR_PRESCALE_8

#define spi_xmit( b )    spi_tx_byte( MAX1303_SPI, b )
typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;

#define RCC_APB2Periph_SPI1              ((uint32_t)0x00001000)

#define CR2_TXEIE     0x00000080
#define CR2_RXNEIE    0x00000040
#define CR2_ERRIE     0x00000020
#define CR2_TXDMAEN   0x00000002
#define CR2_RXDMAEN   0x00000001

typedef struct spi_dev {
  SPI *base;
  GPIO_Port *port;
  uint8 sck_pin;
  uint8 miso_pin;
  uint8 mosi_pin;
} spi_dev;

static const spi_dev spi_dev1 = {
  .base     = (SPI*)SPI1_BASE,
  .port     = GPIOA_BASE,
  .sck_pin  = 5,
  .miso_pin = 6,
  .mosi_pin = 7
};

static const spi_dev spi_dev2 = {
  .base     = (SPI*)SPI2_BASE,
  .port     = GPIOB_BASE,
  .sck_pin  = 13,
  .miso_pin = 14,
  .mosi_pin = 15
};

static void spi_gpio_cfg(const spi_dev *dev) {
  gpio_set_mode(dev->port, dev->sck_pin, GPIO_MODE_AF_OUTPUT_PP);
  gpio_set_mode(dev->port, dev->miso_pin, GPIO_MODE_AF_OUTPUT_PP);
  gpio_set_mode(dev->port, dev->mosi_pin, GPIO_MODE_AF_OUTPUT_PP);
}


void max1303_init( void )
{
  volatile unsigned char dummyread;

  /* Configure I/O for Flash Chip select */
  /*!!AMM investigate the 50MHz flag... */
  gpio_set_mode( MAX1303_CS_BASE, MAX1303_CS, GPIO_MODE_OUTPUT_PP );
  // Other pins are handled by spi_init(...)

    /* De-select the Card: Chip Select high */
  MAX1303_DESELECT();
  
  // This handles GPIO setup on SCK, MISO and MOSI
  // Mode = 0x00 means CPOL = 0, CPHA = 0
 // spi_init( MAX1303_SPI, MAX1303_SPI_PRESCALER, SPI_MSBFIRST, 0x0 );

  timer_set_mode(TIMER3, 2, TIMER_DISABLED);
  timer_set_mode(TIMER3, 1, TIMER_DISABLED);


  uint32 spi_num = MAX1303_SPI;
  uint32 prescale = MAX1303_SPI_PRESCALER;
  uint32 endian = SPI_MSBFIRST;
  uint32 mode = 0;

  SPI *spi;
  uint32 cr1 = 0;

  spi->CR1 &= ~(CR1_SPE);

  switch (spi_num) {
    case 1:
      /* limit to 18 mhz max speed  */
      ASSERT(prescale != CR1_BR_PRESCALE_2);
      spi = (SPI*)SPI1_BASE;
      //rcc_clk_enable(RCC_SPI1);
      //rcc_reset_dev(RCC_SPI1);
      spi_gpio_cfg(&spi_dev1);
      break;
    case 2:
      spi = (SPI*)SPI2_BASE;
      //rcc_clk_enable(RCC_SPI2);
      spi_gpio_cfg(&spi_dev2);
      break;
  }

  cr1 = prescale | endian | mode | CR1_MSTR | CR1_SSI | CR1_SSM;
  spi->CR1 = cr1;
  spi->CR2 &= ~(CR2_TXEIE | CR2_RXNEIE | CR2_ERRIE | CR2_TXDMAEN | CR2_RXDMAEN);

  /* Peripheral enable */
  spi->CR1 |= CR1_SPE;

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
