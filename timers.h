#ifndef TIMERS_H
#define TIMERS_H

#include <xc.h>

extern volatile unsigned char password_timeout_flag;

void init_timer2(void);
void start_password_timer(void);
void stop_password_timer(void);
void timer2_isr(void);

#endif