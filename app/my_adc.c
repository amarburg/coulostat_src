/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 Aaron Marburg
 *                     Cribbed largely from Perry Hung
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
 *
 *  I borrowed liberally and re-made the adc.h code to suit my purposes,
 *  but the libmaple adc support appears to have been significantly
 *  improved.  Perhaps this redundancy isn't required anymore
 */

#include <libmaple.h>
#include <gpio.h>
#include <rcc.h>
#include <nvic.h>

#include "my_adc.h"
#include "adc.h"

volatile bool reference2_adc_updated = false;
volatile int32_t reference2_adc = -100;

volatile bool internal_updated = false;
volatile int16_t internal_v_adc;
volatile int16_t internal_i_adc;

#define INTERNAL_ADC_PORT  GPIOA
#define INTERNAL_V_PIN     0          // Pin PA0 is ADC0, Maple pin 2, 
                                      //       and shared with USART2_CTS
#define INTERNAL_I_PIN     1          // PIN PA1 is ADC1, Maple pin 3,
                                      //       and shared with USART2_RTS
                                      

#define INTERNAL_V_ADC     0
#define INTERNAL_I_ADC     1

#define CR1_DUALMOD_MASK   (BIT_MASK_SHIFT( 0xF, 16 ))
#define CR1_DUALMOD0       (BIT(16))
#define CR1_DUALMOD1       (BIT(17))
#define CR1_DUALMOD2       (BIT(18))
#define CR1_DUALMOD3       (BIT(19))
#define CR1_DUALMOD_REGULAR_SIMULTANEOUS    (CR1_DUALMOD1 | CR1_DUALMOD2)

#define SQR1_CONVERSION_MASK (BIT_MASK_SHIFT( 0x0F, 20))
#define SQR1_2CONVERSION   (BIT(20))

// A few convenience defines on bitbanded bits
#define ADC1_CR2_SWSTART_BB  *(bb_perip( &ADC1->regs->CR2, ADC_CR2_SWSTART_BIT ))
#define ADC2_CR2_RSTCAL_BB   *(bb_perip( &ADC2->regs->CR2, ADC_CR2_RSTCAL_BIT ))
#define ADC2_CR2_CAL_BB      *(bb_perip( &ADC2->regs->CR2, ADC_CR2_CAL_BIT ))


void init_adc( void )
{
   adc_init(ADC1);
   adc_init(ADC2);

   nvic_irq_enable( NVIC_ADC_1_2 );

   ADC2->regs->CR1 = 0;
   // DMA must be set to transfer data from ADC2 to ADC1?
   ADC2->regs->CR2 = ADC_CR2_SWSTART | ADC_CR2_EXTTRIG | ADC_CR2_DMA;

   // Magic numbers taken from libmaple/adc.h
   ADC2->regs->SMPR1 = 0xB6DB6D;
   ADC2->regs->SMPR2 = 0x2DB6DB6D;

   // Enable ADC2 and perform calibration
   adc_enable( ADC2 );
   ADC2_CR2_RSTCAL_BB = 1;
   while( ADC2_CR2_RSTCAL_BB )
     ;
   ADC2_CR2_CAL_BB = 1;
   while( ADC2_CR2_CAL_BB )
     ;

  // With just two channels to measure, use regular simultaneous sampling
  // to spread the load across the two ADCs.  As there's just one channel
  // on each, I can simply grab the data in the IRQ -- don't need DMA
  ADC1->regs->CR1 &= ~(CR1_DUALMOD_MASK);
  ADC1->regs->CR1 |= ADC_CR1_EOCIE_BIT | CR1_DUALMOD_REGULAR_SIMULTANEOUS;

  // Set one conversion for ADC1
  ADC1->regs->SQR1 = 0;
  ADC2->regs->SQR1 = 0;

  ADC1->regs->SQR3 = INTERNAL_V_ADC;
  ADC2->regs->SQR3 = INTERNAL_I_ADC;

   // The Libmaple usart code doesn't use CTS/RTS
   gpio_set_mode( INTERNAL_ADC_PORT, INTERNAL_V_PIN, GPIO_INPUT_ANALOG );
   gpio_set_mode( INTERNAL_ADC_PORT, INTERNAL_I_PIN, GPIO_INPUT_ANALOG );
}

void take_periodic_adc( void )
{
  if( ++reference2_adc > 100 ) reference2_adc = -100;
  reference2_adc_updated = true;

  ADC1_CR2_SWSTART_BB = 1;
}

void __irq_adc( void )
{
  // In dual mode, ADC2's result get's copied to the high 16 bits of ADC_DR
  // Couldn't quite get that working...
  internal_v_adc = ADC1->regs->DR & 0x0000FFFF;
  internal_i_adc = ADC2->regs->DR & 0x0000FFFF;

  internal_updated = true;
}

