/* *****************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 LeafLabs LLC.
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
 * ****************************************************************************/

/**
 *  @brief Sample main.cpp file. Sends "Hello world!" out SPI1.
 *
 *  SPI1 is set up to be a master transmitter at 4.5MHz, little endianness,
 *  and SPI mode 0.
 *
 *  Pin 10 is used as Chip Select
 *
 */

#include "wirish.h"

#define CS      D14     // D14 == PB8

#define LED_PIN D25     // D25 == PD2
uint8 toggle = 0;
uint8 count = 0;

byte buf[] = "Hello world!";

HardwareSPI spi1(1);

void setup() {
  Serial1.begin(115200);
  Serial1.println("Start of setup...");

  pinMode(LED_PIN, OUTPUT);


  /* Set up chip select as output  */
  pinMode(CS, OUTPUT);

  /* CS is usually active low, so initialize it high  */
  digitalWrite(CS, HIGH);

  /* Initialize SPI   */
  spi1.begin(SPI_4_5MHZ, MSBFIRST, 0);
  Serial1.println("Setup end");
}

void loop() {
  //Serial1.println("Loop!");
  Serial1.println( count++ );

  toggle ^= 1;
  digitalWrite(LED_PIN, toggle);


  /* Send message  */
  digitalWrite(CS, LOW);
  spi1.send(buf, sizeof buf);
  digitalWrite(CS,HIGH);
  delay(1000);
}

// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated object that need libmaple may fail.
 __attribute__(( constructor )) void premain() {
    init();
}

int main(void)
{
    setup();

    while (1) {
        loop();
    }
    return 0;
}

