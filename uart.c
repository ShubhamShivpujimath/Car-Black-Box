#include "main.h"
#include "uart.h"

void init_uart(unsigned long baud)
{
    TRISC6 = 0;
    TRISC7 = 1;

    TXSTA = 0x24;
    RCSTA = 0x90;

    SPBRG = (unsigned char)((_XTAL_FREQ / (16UL * baud)) - 1);
}

void uart_putch(unsigned char data)
{
    while (!TXIF);

    TXREG = data;
}

void uart_print(const unsigned char *data)
{
    while (*data)
        uart_putch(*data++);
}