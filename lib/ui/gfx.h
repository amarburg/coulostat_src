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

/* @file gfx.h
 * @brief This is the (vaguely) device-neutral graphics API based on
 *        Jim Lynch's original Nokia 6100 driver code.
 *
 * This should be a hardware-neutral version, shared between the UI code
 * and the hardware interace
 */

#ifndef _GFX_H_
#define _GFX_H_

#include "ui/gfx_conf.h"

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
 

#ifdef __cplusplus
extern "C"{
#endif

void GfxInit(void);
void GfxClearScreen(void);
void GfxSetPixel(int x, int y, int color);
void GfxSetLine(int x0, int y0, int x1, int y1, int color);
void GfxSetRect(int x0, int y0, int x1, int y1, unsigned char fill, int color); 
void GfxSetCircle(int x0, int y0, int radius, int color); 
int GfxPutChar(char c, int x, int y, int size, int fColor, int bColor); 
void GfxPutStr(const char *pString, int x, int y, int Size, int fColor, int bColor); 
void GfxFillScreen( int color );

#ifdef __cplusplus
}
#endif

#endif // Lcd_h 


