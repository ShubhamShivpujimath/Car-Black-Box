#ifndef DIGITAL_KEYPAD_H
#define DIGITAL_KEYPAD_H

#include <xc.h>

#define KEYPAD_PORT       PORTB
#define KEYPAD_PORT_DDR   TRISB
#define INPUT_LINES       0x3F

#define SW1               0x3E
#define SW2               0x3D
#define SW3               0x3B
#define SW4               0x37
#define SW5               0x2F
#define SW6               0x1F

#define ALL_RELEASED      0x3F

#define STATE_CHANGE      0x00
#define LEVEL             0x01

void init_digital_keypad(void);
unsigned char read_digital_keypad(unsigned char mode);

#endif