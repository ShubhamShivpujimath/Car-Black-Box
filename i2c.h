#ifndef I2C_H
#define I2C_H

#include <xc.h>

#define I2C_MASTER_MODE  0x28

void init_i2c(unsigned long clock);
void i2c_start(void);
void i2c_restart(void);
void i2c_stop(void);
unsigned char i2c_write(unsigned char data);
unsigned char i2c_read(unsigned char ack);

#endif