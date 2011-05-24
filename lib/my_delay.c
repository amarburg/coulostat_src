
#include "my_delay.h"
#include <delay.h>

void delay(unsigned long ms) {
  uint32_t i;
  for (i = 0; i < ms; i++) {
    delayMicroseconds(1000);
  }
}

void delayMicroseconds(uint32_t us) {
  delay_us(us);
}

