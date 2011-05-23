
#include <stdlib.h>

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
void Ds1338Rtc::readTimeBytes( uint8_t *buf )
{
my_i2c_read( dev, DS1338_ADDR, DS1338_SEC, 7, buf );
}


// Only works for single bytes
#define BCD2DEC_BYTE(x) (10*( (x&0xF0)>>4 ) + (x&0x0F))
static uint8_t DEC2BCD_BYTE(uint8_t x)
{
  div_t d = div( x,10 );
  return (d.quot << 4) | d.rem;
}

Rtc::RawTime_t &Ds1338Rtc::readTimeRaw( RawTime_t &time )
{
  uint8_t buf[8];
  readTimeBytes( buf );

  // Data is all BCD, whoopee.
  time.sec = BCD2DEC_BYTE( buf[0] & 0x7F );
  time.min = BCD2DEC_BYTE( buf[1] & 0x7F );
  // Just assume it'll be 24hr clock (always use 24hr clock)
  time.hour = BCD2DEC_BYTE( buf[2] & 0x3F );
  time.dow = buf[3] & 0x07;
  time.date = BCD2DEC_BYTE( buf[4] & 0x3F );
  time.month = BCD2DEC_BYTE( buf[5] & 0x1F );
  time.year = BCD2DEC_BYTE( buf[6] );

  return time;
}

void Ds1338Rtc::writeTimeBytes( uint8_t *buf )
{
  my_i2c_write( dev, DS1338_ADDR, DS1338_SEC, 7, buf );
}

void Ds1338Rtc::writeTimeRaw( RawTime_t &time )
{
  uint8_t buf[8];

  buf[0] = DEC2BCD_BYTE( time.sec ) & 0x7F;
  buf[1] = DEC2BCD_BYTE( time.min );
  // Bit 6 low == 24hr mode
  buf[2] = DEC2BCD_BYTE( time.hour ) & 0x3F;
  buf[3] = DEC2BCD_BYTE( time.dow );
  buf[4] = DEC2BCD_BYTE( time.date );
  buf[5] = DEC2BCD_BYTE( time.month );
  buf[6] = DEC2BCD_BYTE( time.year );

  writeTimeBytes( buf );
}

uint8_t Ds1338Rtc::readControl( uint8_t *ctl )
{
  my_i2c_read( dev, DS1338_ADDR, DS1338_CTRL, 1, ctl );
  return ctl[0];
}

