#include "main.h"
#include "i2c.h"

void init_i2c(unsigned long clock)
{
    TRISC3 = 1;
    TRISC4 = 1;

    SSPSTAT = 0x80;
    SSPCON = I2C_MASTER_MODE;
    SSPADD = (unsigned char)((_XTAL_FREQ / (4UL * clock)) - 1);
    SSPCON2 = 0x00;
    SSPIF = 0;
}

void i2c_start(void)
{
    SEN = 1;
    while (SEN);
}

void i2c_restart(void)
{
    RSEN = 1;
    while (RSEN);
}

void i2c_stop(void)
{
    PEN = 1;
    while (PEN);
}

unsigned char i2c_write(unsigned char data)
{
    SSPIF = 0;
    SSPBUF = data;

    while (!SSPIF);

    return ACKSTAT;
}

unsigned char i2c_read(unsigned char ack)
{
    unsigned char data;

    RCEN = 1;
    while (!BF);

    data = SSPBUF;

    ACKDT = ack ? 0 : 1;
    ACKEN = 1;

    while (ACKEN);

    return data;
}