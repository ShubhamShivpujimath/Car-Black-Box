#include "main.h"
#include "adc.h"

void init_adc(void)
{
    ADCON1 = 0x8E;
    ADCON0 = 0x41;
}

unsigned int read_adc(unsigned char channel)
{
    ADCON0 &= 0xC7;
    ADCON0 |= (channel << 3);

    __delay_us(25);

    GO_nDONE = 1;

    while (GO_nDONE);

    return (((unsigned int)ADRESH << 8) | ADRESL);
}