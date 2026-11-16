#include <xc.h>
#include "main.h"
#include "i2c.h"
#include "eeprom.h"

void ext_eeprom_24C02_byte_write(unsigned char addr,
                                 unsigned char data)
{
    i2c_start();

    i2c_write(EXT_EEPROM_WRITE);
    i2c_write(addr);
    i2c_write(data);

    i2c_stop();

    __delay_ms(5);
}

unsigned char ext_eeprom_24C02_byte_read(unsigned char addr)
{
    unsigned char data;

    i2c_start();

    i2c_write(EXT_EEPROM_WRITE);
    i2c_write(addr);

    i2c_restart();

    i2c_write(EXT_EEPROM_READ);

    data = i2c_read(0);

    i2c_stop();

    return data;
}

void ext_eeprom_24C02_str_write(unsigned char addr,
                                unsigned char *data,
                                unsigned char length)
{
    unsigned char i;

    for (i = 0; i < length; i++)
    {
        ext_eeprom_24C02_byte_write(addr + i, data[i]);
    }
}

void ext_eeprom_24C02_str_read(unsigned char addr,
                               unsigned char *data,
                               unsigned char length)
{
    unsigned char i;

    for (i = 0; i < length; i++)
    {
        data[i] = ext_eeprom_24C02_byte_read(addr + i);
    }
}