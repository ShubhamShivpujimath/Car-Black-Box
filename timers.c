#include <xc.h>
#include "timers.h"

volatile unsigned char password_timeout_flag = 0;

static unsigned int password_count = 0;
static unsigned char password_timer_running = 0;

void init_timer2(void)
{
    T2CON = 0x7D;
    PR2 = 249;

    TMR2IF = 0;
    TMR2IE = 1;

    PEIE = 1;
    GIE = 1;
}

void start_password_timer(void)
{
    password_count = 0;
    password_timeout_flag = 0;
    password_timer_running = 1;
}

void stop_password_timer(void)
{
    password_timer_running = 0;
    password_count = 0;
}

void timer2_isr(void)
{
    if (password_timer_running)
    {
        password_count++;

        if (password_count >= 1000)
        {
            password_count = 0;
            password_timer_running = 0;
            password_timeout_flag = 1;
        }
    }
}