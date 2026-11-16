#include "main.h"
#include "i2c.h"
#include "ds1307.h"

void init_ds1307(void)
{
    unsigned char sec;

    sec = read_ds1307(SEC_ADDR);
    write_ds1307(SEC_ADDR, sec & 0x7F);
}

unsigned char read_ds1307(unsigned char addr)
{
    unsigned char data;

    i2c_start();
    i2c_write(DS1307_WRITE);
    i2c_write(addr);

    i2c_restart();
    i2c_write(DS1307_READ);

    data = i2c_read(0);

    i2c_stop();

    return data;
}

void write_ds1307(unsigned char addr, unsigned char data)
{
    i2c_start();
    i2c_write(DS1307_WRITE);
    i2c_write(addr);
    i2c_write(data);
    i2c_stop();

    __delay_ms(5);
}