/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Aaron Marburg
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
 * @file nokia6100.h
 * @brief Definitions and prototypes for accessing NOKIA6100 using USART2
 *
 * The Nokia 6100 (knockoff) LCD screens require 9-bit SPI.  The SPI block 
 * on the STM32 can't do 9-bit SPI, but the USART can.  This code
 * assume USART2.
 *
 * The board I'm using has the EPSON S1G15D00 controller, so this code
 * is written for that module.
 */

#ifndef _NOKIA_6100_H_
#define _NOKIA_6100_H_


#ifdef __cplusplus
extern "C"{
#endif

#define NOKIA_GPIO          GPIOC_BASE

/* LCD reset (active low) is on PC7 */
#define NOKIA_RESET         BIT(7)

/* LCD chip select (active low) is on PC8 */
#define NOKIA_CS            BIT(8)


/**
 * @brief Initialize USART2 for Nokia communications 
 */
void nokia_init( void );


/** 
 * @brief Reset the LCD screen
 */
void nokia_reset( void );

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _USART_H_
