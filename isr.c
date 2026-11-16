#include <xc.h>
#include "timers.h"

void __interrupt() isr(void)
{
    if (TMR2IF)
    {
        TMR2IF = 0;
        timer2_isr();
    }
}