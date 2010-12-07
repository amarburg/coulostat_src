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

#include "nokia6100.h"

/* These defines are redundant with libmaple/usart.c */
#define USART_UE       BIT(13)
#define USART_M        BIT(12)
#define USART_TE       BIT(3)
#define USART_RE       BIT(2)
#define USART_RXNEIE   BIT(5)  // read data register not empty interrupt enable
#define USART_TC       BIT(6)

/**
 * @brief Initialize USART2 for Nokia communications 
 */
void nokia_init( void )
{
    uint8 usart_num = USART2;

    /* Code based loosely on libmaple's usart.[ch] module.  Makes use of some of 
     * the device definition structures defined there.
     */
    ASSERT(usart_num <= NR_USART);
    usart_port *port;
    usart_pins *pins;

    port = usart_dev_table[usart_num].base;
    pins = usart_dev_table[usart_num].pins;
    rcc_clk_enable(usart_dev_table[usart_num].rcc_dev_num);
    nvic_irq_enable(usart_dev_table[usart_num].nvic_dev_num);

    /* Configure the pins appropriately  */
    gpio_set_mode(pins->gpio_port, pins->tx_pin, GPIO_MODE_AF_OUTPUT_PP);
    gpio_set_mode(pins->gpio_port, pins->rx_pin, GPIO_MODE_INPUT_FLOATING);
    gpio_set_mode(pins->gpio_port, pins->ck_pin, GPIO_MODE_OUTPUT_PP);

    gpio_set_mode( NOKIA_GPIO, NOKIA_CS, GPIO_MODE_OUTPUT_PP );
    gpio_set_mode( NOKIA_GPIO, NOKIA_RESET, GPIO_MODE_OUTPUT_PP );

    if ((usart_num == USART1) ||
        (usart_num == USART2)) {
      /* turn off any pwm if there's a conflict on this usart */
      timer_set_mode(pin->timer_num, pin->compare_num, TIMER_DISABLED);
    }

    /* set clock rate to 6MHz */
    port->BRR = 36000000L/6000000L; //APB1 bus max speed is 36MHz
    port->CR1 |= (USART_RE | USART_TE | USART_M); // RX, TX enable, 9-bit
    port->CR2 |= USART2_CR2_CLKEN; //enable SCK pin CPOL and CPHA are also in this register



    port->CR1 |= USART_UE; // USART enable
}

/** 
 * @brief Reset the LCD screen
 */
void nokia_reset( void ) {
  uint8_t usart_num = USART2;
  gpio_write_bit( NOKIA_GPIO, NOKIA_RESET, 0 );
  delay( 20000 );
  gpio_write_bit( NOKIA_GPIO, NOKIA_RESET, 1 );
  delay( 20000 );

}


