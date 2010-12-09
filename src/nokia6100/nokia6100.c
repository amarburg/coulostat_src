/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Aaron Marburg.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

/**
 * @brief USART control routines
 */

#include "libmaple.h"
#include "rcc.h"
#include "nvic.h"
#include "usart.h"
#include "timers.h"

/* header files from wirish */
#include "time.h"

#include "nokia6100.h"

/* These defines are redundant with libmaple/usart.c */
#define USART_UE       BIT(13)
#define USART_M        BIT(12)
#define USART_TE       BIT(3)
#define USART_RE       BIT(2)
#define USART_RXNEIE   BIT(5)  // read data register not empty interrupt enable
#define USART_TC       BIT(6)

/* In control register2 (USART_CR2) */
#define USART_CLKEN    BIT(11)
#define USART_CPOL     BIT(10)
#define USART_CPHA     BIT(9)



/**
 * @brief Enables (pulls low) the chip select for the display
 */
static inline void nokia_enable_cs( void ) {
  gpio_write_bit( NOKIA_GPIO, NOKIA_CS, 0 );
}

/**
 * @brief Disables (pulls high) the chip select for the display
 */
static inline void nokia_disable_cs( void ) {
  gpio_write_bit( NOKIA_GPIO, NOKIA_CS, 1 );
}

/** 
 *
 * As noted elsewhere, SPI is MSB first, while USART is LSB first (?)
 * The following code from the StdPeriph library uses a 1-cycle Thumb
 * instruction to reverse data
 */

/**
 * * @brief  Reverse bit order of value
 * *
 * * @param  uint32_t value to reverse
 * * @return uint32_t reversed value
 * *
 * * Reverse bit order of value
 * */
uint32 __RBIT(uint32 value)
{
    __asm__("rbit r0, r0");
    __asm__("bx lr");
}

/**
 * @brief Initialize USART2 for Nokia communications 
 */
void nokia_init( void )
{
    /* Code based loosely on libmaple's usart.[ch] module.  Makes use of some of 
     * the device definition structures defined there.
    */
    ASSERT(NOKIA_USART <= NR_USART);
    usart_port *port;
    usart_pins *pins;

    usart_init( USART2, 9600 );// 6000000UL );
    port = usart_dev_table[NOKIA_USART].base;
    pins = usart_dev_table[NOKIA_USART].pins;

    /*uint32 clk_speed;
    uint32 integer_part;
    uint32 fractional_part;
    uint32 tmp;
    uint32 baud = 6000000UL;

    rcc_clk_enable(usart_dev_table[NOKIA_USART].rcc_dev_num);
    nvic_irq_enable(usart_dev_table[NOKIA_USART].nvic_dev_num); */

    /* Configure the pins appropriately  */
    gpio_set_mode(pins->gpio_port, pins->tx_pin, GPIO_MODE_AF_OUTPUT_PP);
    gpio_set_mode(pins->gpio_port, pins->rx_pin, GPIO_MODE_INPUT_FLOATING);
    gpio_set_mode(pins->gpio_port, pins->ck_pin, GPIO_MODE_AF_OUTPUT_PP); 

    gpio_set_mode( NOKIA_GPIO, NOKIA_CS, GPIO_MODE_OUTPUT_PP );
    gpio_set_mode( NOKIA_GPIO, NOKIA_RESET, GPIO_MODE_OUTPUT_PP );

    if ((NOKIA_USART == USART1) ||
        (NOKIA_USART == USART2)) {
      /* turn off any pwm if there's a conflict on this usart */
      timer_set_mode(pins->timer_num, pins->compare_num, TIMER_DISABLED);
    }

    /* set clock rate to 6MHz */
    //port->CR1 |= USART_UE;
    //port->BRR = 36000000L/6000000L; //APB1 bus max speed is 36MHz
        /* Set baud rate  */

    /*clk_speed = 36000000UL;
    integer_part = ((25 * clk_speed) / (4 * baud));
    tmp = (integer_part / 100) << 4;

    fractional_part = integer_part - (100 * (tmp >> 4));
    tmp |= (((fractional_part * 16) + 50) / 100) & ((uint8)0x0F);

    port->BRR = (uint16)tmp;
    port->CR1 |= (USART_RE | USART_TE | USART_M); */// RX, TX enable, 9-bit

    port->CR2 |= USART_CLKEN; //enable SCK pin CPOL and CPHA are also in this register
    port->CR2 &= ~(USART_CPOL | USART_CPHA);

    nokia_disable_cs();
    port->CR1 |= USART_UE; // USART enable
}

/** 
 * @brief Reset the LCD screen
 */
void nokia_reset( void ) {
  gpio_write_bit( NOKIA_GPIO, NOKIA_RESET, 0 );
  delay( 20 );
  gpio_write_bit( NOKIA_GPIO, NOKIA_RESET, 1 );
  delay( 20 );
}

/**
 *
 * Generic function to write to USART
 */
static inline void nokia_write_spi( uint32 data ) {
  nokia_enable_cs();
  usart_port *port = usart_dev_table[NOKIA_USART].base;
  uint32 a = port->SR;
  port->DR = data & 0x000001FF;
  /* As we're controlling chip select, need to wait for the whole data
   * frame to be transmitted, so we watch TC, not TXE */
  while ((port->SR & USART_TC) == 0) ;
  nokia_disable_cs();
}

/**
 *
 * Bit 8 is clear for commands 
 */
void nokia_write_spi_command( uint32 data) {
  uint32 out = __RBIT( data & 0x000000FF ) >> 23;
  nokia_write_spi( out );
}

/**
 * bit 8 is set for data
 *
 */
void nokia_write_spi_data( uint32 data) { 
  uint32 out = __RBIT( (data & 0x000000FF) | 0x000000100 ) >> 23;
  nokia_write_spi( out );
}


