
#include "rtc.h"
#include "my_i2c.h"
#include "libmaple.h"
#include "i2c.h"

#include "ds1338.h"

Ds1338Rtc::Ds1338Rtc( I2C bus )
{
  switch( bus ) {
  case I2C_1: dev = I2C1; break;
  case I2C_2: dev = I2C1; break;
  }

  i2c_master_enable( dev, I2C_FAST_MODE | I2C_BUS_RESET );
}


void Ds1338Rtc::startClock()
{
  unsigned char secs[2];
  my_i2c_read( dev, DS1338_ADDR, DS1338_SEC, 1, secs );
  secs[0] |= DS1338_CH;
  my_i2c_write( dev, DS1338_ADDR, DS1338_SEC, 1, secs );
}

void Ds1338Rtc::stopClock()
{
  unsigned char secs[2];
  my_i2c_read( dev, DS1338_ADDR, DS1338_SEC, 1, secs );
  secs[0] &= ~(DS1338_CH);
  my_i2c_write( dev, DS1338_ADDR, DS1338_SEC, 1, secs );
}

bool Ds1338Rtc::isClockRunning()
{
  unsigned char secs[2];
  my_i2c_read( dev, DS1338_ADDR, DS1338_SEC, 1, secs );

  return secs[0] & DS1338_CH;
}


int Ds1338Rtc::readRam( uint8_t offset, uint8_t len, uint8_t *buf )
{
  if( (offset+len) > DS1338_RAM_SIZE ) return -1;
  return my_i2c_read( dev, DS1338_ADDR, DS1338_RAM_BASE+offset, len, buf );
}

int Ds1338Rtc::writeRam( uint8_t offset, uint8_t len, uint8_t *buf )
{
  if( (offset+len) > DS1338_RAM_SIZE ) return -1;
  return my_i2c_write( dev, DS1338_ADDR, DS1338_RAM_BASE+offset, len, buf );
}

  // Buf must be at least 7 bytes
void Ds1338Rtc::readTimeRaw( uint8_t *buf )
{
my_i2c_read( dev, DS1338_ADDR, DS1338_SEC, 7, buf );
}

uint8_t Ds1338Rtc::readControl( uint8_t *ctl )
{
  my_i2c_read( dev, DS1338_ADDR, DS1338_CTRL, 1, ctl );
  return ctl[0];
}

