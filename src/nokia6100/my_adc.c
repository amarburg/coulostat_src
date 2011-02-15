/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 Aaron Marburg
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
 *
 *  @brief My analog-to-digital conversion routines.  Not to be confused with
 *  adc.h in the libmaple source.
 */

#include <libmaple.h>
#include <gpio.h>

#include "my_adc.h"
#include "adc.h"

volatile bool reference2_adc_updated = false;
int32_t reference2_adc = 0;

#define INTERNAL_ADC_PORT  GPIOA_BASE
#define INTERNAL_V_PIN     0          // Pin PA0 is ADC0, Maple pin 2, 
                                      //       and shared with USART2_CTS
#define INTERNAL_I_PIN     1          // PIN PA1 is ADC1, Maple pin 3,
                                      //       and shared with USART2_RTS
                                      

#define INTERNAL_V_ADC     0
#define INTERNAL_I_ADC

void init_adc( void )
{
   adc_init();

   // The Libmaple usart code doesn't use CTS/RTS
   
   gpio_set_mode( INTERNAL_ADC_PORT, INTERNAL_V_PIN, GPIO_MODE_INPUT_ANALOG );
   gpio_set_mode( INTERNAL_ADC_PORT, INTERNAL_I_PIN, GPIO_MODE_INPUT_ANALOG );
   
}

