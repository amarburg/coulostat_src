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
#include <rcc.h>
#include <nvic.h>

#include "my_adc.h"
#include "adc.h"

volatile bool reference2_adc_updated = false;
volatile int32_t reference2_adc = -100;

volatile bool internal_updated = false;
volatile int16_t internal_v_adc;
volatile int16_t internal_i_adc;

#define INTERNAL_ADC_PORT  GPIOA_BASE
#define INTERNAL_V_PIN     0          // Pin PA0 is ADC0, Maple pin 2, 
                                      //       and shared with USART2_CTS
#define INTERNAL_I_PIN     1          // PIN PA1 is ADC1, Maple pin 3,
                                      //       and shared with USART2_RTS
                                      

#define INTERNAL_V_ADC     0
#define INTERNAL_I_ADC     1


#ifdef ADC2_BASE
#undef ADC2_BASE
#define ADC2_BASE                 0x40012800  
#endif
#define ADC2_SR                   *(volatile uint32*)(ADC2_BASE + 0)
#define ADC2_CR1                  *(volatile uint32*)(ADC2_BASE + 0x4)
#define ADC2_CR2                  *(volatile uint32*)(ADC2_BASE + 0x8)
#define ADC2_SMPR1                *(volatile uint32*)(ADC2_BASE + 0xC)
#define ADC2_SMPR2                *(volatile uint32*)(ADC2_BASE + 0x10)
#define ADC2_SQR1                 *(volatile uint32*)(ADC2_BASE + 0x2C)
#define ADC2_SQR3                 *(volatile uint32*)(ADC2_BASE + 0x34)
#define ADC2_DR                   *(volatile uint32*)(ADC2_BASE + 0x4C)

#define CR1_DUALMOD_MASK   (BIT_MASK_SHIFT( 0xF, 16 ))
#define CR1_DUALMOD0       (BIT(16))
#define CR1_DUALMOD1       (BIT(17))
#define CR1_DUALMOD2       (BIT(18))
#define CR1_DUALMOD3       (BIT(19))
#define CR1_DUALMOD_REGULAR_SIMULTANEOUS    (CR1_DUALMOD1 | CR1_DUALMOD2)

#define CR1_EOCIE          (BIT(5))

#define CR2_ADON           (BIT(0))
#define CR2_CAL            (BIT(2))
#define CR2_RSTCAL         (BIT(3))
#define CR2_DMA            (BIT(8))
#define CR2_SWSTART        (BIT(22))

#define SQR1_CONVERSION_MASK (BIT_MASK_SHIFT( 0x0F, 20))
#define SQR1_2CONVERSION   (BIT(20))

// Add'l ADC1 bitbanded bits
#define ADC_CR1_EOCIE_BIT      *(volatile uint32*)(BITBAND_PERI(ADC1_BASE+0x4, 5))

//ADC2 bitbanded bits
#define ADC2_CR2_ADON_BIT    *(volatile uint32*)(BITBAND_PERI(ADC2_BASE+0x8, 0))
#define ADC2_CR2_CAL_BIT     *(volatile uint32*)(BITBAND_PERI(ADC2_BASE+0x8, 2))
#define ADC2_CR2_RSTCAL_BIT  *(volatile uint32*)(BITBAND_PERI(ADC2_BASE+0x8, 3))
#define ADC2_CR2_SWSTART_BIT *(volatile uint32*)(BITBAND_PERI(ADC2_BASE+0x8 + 2, 6))
#define ADC2_SR_EOC_BIT      *(volatile uint32*)(BITBAND_PERI(ADC2_BASE+0, 1))


void init_adc( void )
{
   adc_init();

   // Enable ADC2
   rcc_clk_enable(RCC_ADC2);
   rcc_reset_dev(RCC_ADC2);

   nvic_irq_enable( NVIC_ADC1_2 );

   ADC2_CR1 = 0;
   // DMA must be set to transfer data from ADC2 to ADC1?
   ADC2_CR2 = CR2_EXTSEL_SWSTART | CR2_EXTTRIG | CR2_DMA;

   // Magic numbers taken from libmaple/adc.h
   ADC2_SMPR1 = 0xB6DB6D;
   ADC2_SMPR2 = 0x2DB6DB6D;

   // Enable ADC2 and perform calibration
   ADC2_CR2_ADON_BIT = 1;
   ADC2_CR2_RSTCAL_BIT = 1;
   while( ADC2_CR2_RSTCAL_BIT )
     ;
   ADC2_CR2_CAL_BIT = 1;
   while( ADC2_CR2_CAL_BIT )
     ;

  // With just two channels to measure, use regular simultaneous sampling
  // to spread the load across the two ADCs.  As there's just one channel
  // on each, I can simply grab the data in the IRQ -- don't need DMA
  ADC_CR1 = (ADC_CR1 & ~(CR1_DUALMOD_MASK)) | CR1_DUALMOD_REGULAR_SIMULTANEOUS 
    | CR1_EOCIE;

  // Set one conversion for ADC1
  ADC_SQR1 = 0;
  ADC2_SQR1 = 0;

  ADC_SQR3 = INTERNAL_V_ADC;
  ADC2_SQR3 = INTERNAL_I_ADC;

   // The Libmaple usart code doesn't use CTS/RTS
   gpio_set_mode( INTERNAL_ADC_PORT, INTERNAL_V_PIN, GPIO_MODE_INPUT_ANALOG );
   gpio_set_mode( INTERNAL_ADC_PORT, INTERNAL_I_PIN, GPIO_MODE_INPUT_ANALOG );
}

void take_periodic_adc( void )
{
  reference2_adc++;
  if( reference2_adc > 100 ) reference2_adc = -100;
  reference2_adc_updated = true;

  CR2_SWSTART_BIT = 1;
}

void ADC_IRQHandler( void )
{
  // In dual mode, ADC2's result get's copied to the high 16 bits of ADC_DR
  // Couldn't quite get that working...
  internal_v_adc = ADC_DR & 0x0000FFFF;
  internal_i_adc = ADC2_DR & 0x0000FFFF;

  internal_updated = true;
}

