//
// Device-specific defines for Maxim DS1338 I2C RTC chip
//

#ifndef __DS1338_H__
#define __DS1338_H__

#ifdef __cplusplus
extern "C" {
#endif

#define DS1338_SEC      0x00
#define DS1338_MIN      0x01
#define DS1338_HOUR     0x02
#define DS1338_DAY      0x03
#define DS1338_MONTH    0x05
#define DS1338_YEAR     0x06
#define DS1338_CTRL     0x07

#define DS1338_RAM_BASE 0x08
#define DS1338_RAM_SIZE 0x38  

#define DS1338_ADDR  0xD0

// The CH (clock halt) bit is in the DS1338_SEC register
#define DS1338_CH       (1<<7)

#define DS1338_SQWOUT   (1<<7)
#define DS1338_OSF      (1<<5)
#define DS1338_SQWE     (1<<4)
#define DS1338_SQWRS1   (1<<1)
#define DS1338_SQWRS0   (1<<0)


#ifdef __cplusplus
}
#endif
#endif
