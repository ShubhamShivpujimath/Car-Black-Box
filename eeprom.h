#ifndef EEPROM_H
#define EEPROM_H

#define EXT_EEPROM_WRITE     0xA0
#define EXT_EEPROM_READ      0xA1

void ext_eeprom_24C02_byte_write(unsigned char addr,
                                 unsigned char data);

unsigned char ext_eeprom_24C02_byte_read(unsigned char addr);

void ext_eeprom_24C02_str_write(unsigned char addr,
                                unsigned char *data,
                                unsigned char length);

void ext_eeprom_24C02_str_read(unsigned char addr,
                               unsigned char *data,
                               unsigned char length);

#endif