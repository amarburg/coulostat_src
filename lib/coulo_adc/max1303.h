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
 *  @brief Defines and control functions for the MAX1303 SPI ADC
 */

#ifndef __MAX1303_H__
#define __MAX1303_H__

#include <stdint.h>
#include <stdbool.h>

// Hardware configuration
//
#define MAX1303_CS_BASE   GPIOB
#define MAX1303_CS        8

#define MAX1303_SPI       SPI1

// This is provided in wirish, but not  at the C level.  Why not?
#define MAX1303_SPI_BASE  GPIOA
#define MAX1303_SPI_SCK   5
#define MAX1303_SPI_MISO  6
#define MAX1303_SPI_MOSI  7

#define MAX1303_APBPeriphClockCmd_SPI   RCC_APB2PeriphClockCmd
#define MAX1303_APBPeriph_SPI           RCC_APB2Perish_SPI1

#define MAX1303_MODE      MODE_EXTERNAL_CLOCK


#define CHANNEL_SHIFT 4
#define MAX1303_CHAN0 (0x00 << CHANNEL_SHIFT)
#define MAX1303_CHAN1 (0x01 << CHANNEL_SHIFT)
#define MAX1303_CHAN2 (0x02 << CHANNEL_SHIFT)
#define MAX1303_CHAN3 (0x03 << CHANNEL_SHIFT)
#define MAX1303_CHAN_MASK 0x70

#define DIFFERENTIAL (0x01 << 3)

#define SE_PLUS_MINUS_VREF_4  0x01
#define SE_MINUS_VREF_2       0x02
#define SE_PLUS_VREF_2        0x03
#define SE_PLUS_MINUS_VREF_2  0x04
#define SE_MINUS_VREF         0x05
#define SE_PLUS_VREF          0x06
#define SE_PLUS_MINUS_VREF    0x07
#define MAX1303_RANGE_MASK    0x0F

#define DIFF_PLUS_MINUS_VREF_2  (DIFFERENTIAL | 0x01)
#define DIFF_PLUS_MINUS_VREF    (DIFFERENTIAL | 0x04)
#define DIFF_PLUS_MINUS_2_VREF  (DIFFERENTIAL | 0x07)

extern void max1303_analog_input_config( unsigned char chan, unsigned char config );


#define MODE_EXTERNAL_CLOCK   (0x00)
#define MODE_EXTERNAL_ACQ     (0x01)
#define MODE_INTERNAL_CLOCK   (0x02)
#define MODE_RESET            (0x04)
#define MODE_PARTIAL_POWER_DOWN (0x05)
#define MODE_FULL_POWER_DOWN  (0x07)

extern void max1303_mode_config( unsigned char mode );

extern void max1303_init( void );

extern void max1303_partial_power_down( void );
extern void max1303_full_power_down( void );
extern void max1303_wake( void );

#ifdef __cplusplus
extern "C" {
#endif
  bool is_acq_completed( void );
  int8_t max1303_acq_external_clock_nonblocking( uint8_t chans, uint8_t oversample, uint16_t *data );
  int8_t max1303_acq_external_clock_blocking( uint8_t chans, uint8_t oversample, uint16_t *data );
#ifdef __cplusplus
}
#endif

#define MAX1303_SELECT()         gpio_write_bit( MAX1303_CS_BASE, MAX1303_CS, 0 ) 
#define MAX1303_DESELECT()       gpio_write_bit( MAX1303_CS_BASE, MAX1303_CS, 1 ) 



#endif
