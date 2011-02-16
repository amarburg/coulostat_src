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
 *  @file adc.h
 *
 *  @brief My analog-to-digital conversion routines.  Not to be confused with
 *  adc.h in the libmaple source.
 */

#ifndef _MY_ADC_H_
#define _MY_ADC_H_

#include <stdint.h>
#include <stdbool.h>

#define REFERENCE_SCALER 0.001
extern volatile bool reference2_adc_updated; 
extern volatile int32_t reference2_adc;

extern volatile bool internal_updated;
extern volatile int16_t internal_v_adc;
extern volatile int16_t internal_i_adc;

#ifdef __cplusplus
extern "C" {
#endif

  void init_adc( void );

  // Will be called from IRQs (systick handler, mostly)
  void take_periodic_adc( void );

#ifdef __cplusplus
}
#endif

#endif
