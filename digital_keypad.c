#include "main.h"
#include "digital_keypad.h"

void init_digital_keypad(void)
{
    KEYPAD_PORT_DDR |= INPUT_LINES;
    OPTION_REGbits.nRBPU = 0;
}

unsigned char read_digital_keypad(unsigned char mode)
{
    static unsigned char once = 1;
    unsigned char key = KEYPAD_PORT & INPUT_LINES;

    if (mode == STATE_CHANGE)
    {
        if ((key != ALL_RELEASED) && once)
        {
            once = 0;
            return key;
        }

        if (key == ALL_RELEASED)
            once = 1;

        return ALL_RELEASED;
    }

    return key;
}