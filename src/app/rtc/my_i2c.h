

#ifndef __MY_I2C_H__
#define __MY_I2C_H__

#include <stdint.h>

#include "libmaple.h"
#include "gpio.h"
#include "i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

int my_i2c_read( i2c_dev *dev, uint8_t addr, 
    uint8_t register_addr,
    uint8_t len, uint8_t *buf );
int my_i2c_write( i2c_dev *dev, uint8_t addr, 
    uint8_t register_addr,
    uint8_t len, uint8_t *buf );

#ifdef __cplusplus
}
#endif
#endif
