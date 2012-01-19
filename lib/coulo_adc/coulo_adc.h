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
 *  @brief Functions related to the coulostat ADC phase.  More application-specific
 *  than max1303.h
 */


#ifndef __COULO_ADC_H__
#define __COULO_ADC_H__

#include <stdint.h>

typedef uint16_t coulo_adc_result_t[4];


#define COULO_ADC_0   (0x01)
#define COULO_ADC_1   (0x02)
#define COULO_ADC_2   (0x04)
#define COULO_ADC_3   (0x08)

#define COULO_ADC_012    (COULO_ADC_0 | COULO_ADC_1 | COULO_ADC_2)
#define COULO_ADC_0123   (COULO_ADC_0 | COULO_ADC_1 | COULO_ADC_2 | COULO_ADC_3)
#define COULO_ADC_ALL    COULO_ADC_0123

#ifdef __cplusplus
extern "C" {
#endif
  void coulo_adc_init( void );
  int8_t coulo_adc_read( unsigned char chans, uint16_t *results );
#ifdef __cplusplus
}
#endif


#endif
