
#include <stdint.h>
#include <string.h>

#include "my_i2c.h"

int my_i2c_read( i2c_dev *dev, uint8_t i2c_addr,
    uint8_t register_addr,
    uint8_t len, uint8_t *buf )
{
  unsigned char myTmp[2];
  i2c_msg msgs[2] = {{ .addr = i2c_addr,
                  .length = 1,
                  .flags = 0,
                  .data = myTmp }, 
                    { .addr = i2c_addr,
                      .length = len,
                      .flags = I2C_MSG_READ,
                      .data = buf } };
  myTmp[0] = register_addr;
  i2c_master_xfer( dev, msgs, 2, 1000 );

  return msgs[0].xferred;
}

int my_i2c_write( i2c_dev *dev, uint8_t i2c_addr,
    uint8_t register_addr,
    uint8_t len, uint8_t *buf )
{
  unsigned char myTmp[64];
  i2c_msg msgs[1] = {{ .addr = i2c_addr,
                  .length = len+1,
                  .flags = 0,
                  .data = myTmp }  };
  myTmp[0] = register_addr;
  strncpy( &(myTmp[1]), buf, len );
  i2c_master_xfer( dev, msgs, 1, 1000 );

  return msgs[0].xferred;
}
