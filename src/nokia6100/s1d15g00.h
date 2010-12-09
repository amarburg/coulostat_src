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

/* @file s1d15g00.h
 * @brief Graphics driver Epson S1D15G00 LCD controller
 *
 * Lifted heavily from James Lynch's August 30 2007 code.
 *
 * Contains code specific to the Espon controller.  Requires the following
 * hardware/SPI-specific functions to be defined elsewhere.
 */

#ifndef _S1D15G00_H_
#define _S1D15G00_H_

// backlight control 
#define BKLGHT_LCD_ON 1 
#define BKLGHT_LCD_OFF 2 
 
// Booleans 
#define NOFILL 0 
#define FILL 1 
 
// 12-bit color definitions 
#define WHITE 0xFFF 
#define BLACK 0x000 
#define RED 0xF00 
#define GREEN 0x0F0 
#define BLUE 0x00F 
#define CYAN 0x0FF 
#define MAGENTA 0xF0F 
#define YELLOW 0xFF0 
#define BROWN 0xB22 
#define ORANGE 0xFA0 
#define PINK 0xF6A 
 
// Font sizes 
#define SMALL 0 
#define MEDIUM 1 
#define LARGE 2 
 
// Define LCD-NOKIA6610 PinIO Interface Mask Bit  
 
// End of Define For LCD-NOKIA6610 

#ifndef BIT0
// mask definitions 
#define BIT0  0x00000001 
#define BIT1  0x00000002 
#define BIT2  0x00000004 
#define BIT3  0x00000008 
#define BIT4  0x00000010 
#define BIT5  0x00000020 
#define BIT6  0x00000040 
#define BIT7  0x00000080 
#define BIT8  0x00000100 
#define BIT9  0x00000200 
#define BIT10 0x00000400 
#define BIT11 0x00000800 
#define BIT12 0x00001000 
#define BIT13 0x00002000 
#define BIT14 0x00004000 
#define BIT15 0x00008000 
#define BIT16 0x00010000 
#define BIT17 0x00020000 
#define BIT18 0x00040000 
#define BIT19 0x00080000 
#define BIT20 0x00100000 
#define BIT21 0x00200000 
#define BIT22 0x00400000 
#define BIT23 0x00800000 
#define BIT24 0x01000000 
#define BIT25 0x02000000 
#define BIT26 0x04000000 
#define BIT27 0x08000000 
#define BIT28 0x10000000 
#define BIT29 0x20000000 
#define BIT30 0x40000000 
#define BIT31 0x80000000 
#endif

#ifdef __cplusplus
extern "C"{
#endif

void InitLcd(void);
void LCDClearScreen(void);
void LCDSetPixel(int x, int y, int color);
void LCDSetLine(int x0, int y0, int x1, int y1, int color);
void LCDSetRect(int x0, int y0, int x1, int y1, unsigned char fill, int color); 
void LCDSetCircle(int x0, int y0, int radius, int color); 
void LCDPutChar(char c, int x, int y, int size, int fColor, int bColor); 
void LCDPutStr(char *pString, int x, int y, int Size, int fColor, int bColor); 

#ifdef __cplusplus
}
#endif

#endif // Lcd_h 


