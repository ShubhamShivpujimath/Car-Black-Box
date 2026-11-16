#ifndef UART_H
#define UART_H

#include <xc.h>

void init_uart(unsigned long baud);
void uart_putch(unsigned char data);
void uart_print(const unsigned char *data);

#endif