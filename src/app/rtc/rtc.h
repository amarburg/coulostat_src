//
// C++ RTC class
//

#ifndef __RTC_H__
#define __RTC_H__

#include <stdint.h>

#include "libmaple.h"
#include "gpio.h"
#include "i2c.h"

class Rtc {
public:
  Rtc() {;}
};


class Ds1338Rtc : public Rtc {
public:
  enum I2C {I2C_1, I2C_2};

  Ds1338Rtc(  I2C bus );

  void startClock();
  void stopClock();
  bool isClockRunning();

  int readRam( uint8_t offset, uint8_t len, uint8_t *buf );
  int writeRam( uint8_t offset, uint8_t len, uint8_t *buf );


  // Buf must be at least 7 bytes
  void readTimeRaw( uint8_t *buf );

  uint8_t readControl( uint8_t *ctl );
private:

  i2c_dev *dev;

};



#ifndef __RTC_H__
#define __RTC_H__
#endif

#endif
