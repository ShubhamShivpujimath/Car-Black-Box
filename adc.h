#ifndef ADC_H
#define ADC_H

#include <xc.h>

#define CHANNEL0  0x00

void init_adc(void);
unsigned int read_adc(unsigned char channel);

#endif