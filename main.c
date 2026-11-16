/*
 * File:   main.c
 * Author: SHUBHAM SHIVPUJIMATH
 *
 * Created on 14 August, 2026, 9:42 PM
 */

#include <xc.h>

#include "main.h"
#include "clcd.h"
#include "adc.h"
#include "i2c.h"
#include "ds1307.h"
#include "digital_keypad.h"
#include "timers.h"
#include "uart.h"
#include "cbb.h"

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

static unsigned char get_key(void)
{
    unsigned char key;

    key = PORTB & INPUT_LINES;

    if (key == ALL_RELEASED)
        return ALL_RELEASED;

    __delay_ms(30);

    key = PORTB & INPUT_LINES;

    while ((PORTB & INPUT_LINES) != ALL_RELEASED);

    __delay_ms(30);

    return key;
}

void main(void)
{
    unsigned char key;
    unsigned char screen = DASH_BOARD_SCREEN;
    unsigned char reset_flag = RESET_NOTHING;
    unsigned char password_status;
    unsigned char menu_option = 0;

    unsigned char event[2] = {'O', 'N'};
    unsigned char speed;

    init_clcd();
    init_digital_keypad();
    init_adc();
    init_i2c(100000);
    init_ds1307();
    init_timer2();
    init_uart(9600);
    init_password();

    clear_screen();

    while (1)
    {
        key = get_key();

        switch (screen)
        {
            case DASH_BOARD_SCREEN:

                speed = read_adc(CHANNEL0) / 10;

                if (speed > 99)
                    speed = 99;

                if (key == SW1)
                {
                    /* Gr++ :  ON -> GN -> GR -> G1 -> G2 -> G3 -> G4 -> ON */

                    if (event[0] == 'O' && event[1] == 'N')
                    {
                        event[0] = 'G';
                        event[1] = 'N';
                    }
                    else if (event[0] == 'G' && event[1] == 'N')
                    {
                        event[0] = 'G';
                        event[1] = 'R';
                    }
                    else if (event[0] == 'G' && event[1] == 'R')
                    {
                        event[0] = 'G';
                        event[1] = '1';
                    }
                    else if (event[0] == 'G' && event[1] == '1')
                    {
                        event[1] = '2';
                    }
                    else if (event[0] == 'G' && event[1] == '2')
                    {
                        event[1] = '3';
                    }
                    else if (event[0] == 'G' && event[1] == '3')
                    {
                        event[1] = '4';
                    }
                    else
                    {
                        event[0] = 'O';
                        event[1] = 'N';
                    }

                    log_event(event, speed);
                }
                else if (key == SW2)
                {
                    /* Gr-- :  ON -> G4 -> G3 -> G2 -> G1 -> GR -> GN -> ON */

                    if (event[0] == 'O' && event[1] == 'N')
                    {
                        event[0] = 'G';
                        event[1] = '4';
                    }
                    else if (event[0] == 'G' && event[1] == '4')
                    {
                        event[1] = '3';
                    }
                    else if (event[0] == 'G' && event[1] == '3')
                    {
                        event[1] = '2';
                    }
                    else if (event[0] == 'G' && event[1] == '2')
                    {
                        event[1] = '1';
                    }
                    else if (event[0] == 'G' && event[1] == '1')
                    {
                        event[1] = 'R';
                    }
                    else if (event[0] == 'G' && event[1] == 'R')
                    {
                        event[1] = 'N';
                    }
                    else
                    {
                        event[0] = 'O';
                        event[1] = 'N';
                    }

                    log_event(event, speed);
                }
                else if (key == SW3)
                {
                    /* Clutch: momentary log entry, current gear unchanged */

                    unsigned char clutch_event[2] = {'C', ' '};

                    log_event(clutch_event, speed);
                }
                else if (key == SW6)
                {
                    screen = PASSWORD_SCREEN;
                    reset_flag = RESET_PASSWORD_FLAG;
                    clear_screen();
                }

                display_dashboard(event, speed);
                break;


            case PASSWORD_SCREEN:

                if (reset_flag == RESET_PASSWORD_FLAG)
                {
                    check_password(ALL_RELEASED,
                                   RESET_PASSWORD_FLAG);

                    reset_flag = RESET_NOTHING;
                    break;
                }

                password_status = check_password(key, RESET_NOTHING);

                if (password_status == RETURN_SUCCESS)
                {
                    screen = MENU_SCREEN;
                    reset_flag = RESET_MENU_SCREEN;
                    clear_screen();
                }
                else if (password_status == RETURN_FAIL ||
                         password_status == TIME_OUT)
                {
                    screen = DASH_BOARD_SCREEN;
                    reset_flag = RESET_NOTHING;
                    clear_screen();
                }

                break;


            case MENU_SCREEN:

                menu_option = menu_screen(key, reset_flag);
                reset_flag = RESET_NOTHING;

                if (key == SW6)
                {
                    if (menu_option == 0)
                    {
                        screen = VIEW_LOG_SCREEN;
                    }
                    else if (menu_option == 1)
                    {
                        screen = CLEAR_LOG_SCREEN;
                    }
                    else if (menu_option == 2)
                    {
                        screen = DOWNLOAD_LOG_SCREEN;
                    }
                    else if (menu_option == 3)
                    {
                        screen = SET_TIME_SCREEN;
                    }
                    else if (menu_option == 4)
                    {
                        screen = CHANGE_PASSWORD_SCREEN;
                    }

                    reset_flag = RESET_MENU_SCREEN;
                    clear_screen();
                }

                break;


            case VIEW_LOG_SCREEN:

                view_log(key, reset_flag);
                reset_flag = RESET_NOTHING;

                if (key == SW2)
                {
                    screen = MENU_SCREEN;
                    reset_flag = RESET_MENU_SCREEN;
                    clear_screen();
                }

                break;


            case CLEAR_LOG_SCREEN:

                clear_log_screen(key, reset_flag);
                reset_flag = RESET_NOTHING;

                if (key == SW2)
                {
                    screen = MENU_SCREEN;
                    reset_flag = RESET_MENU_SCREEN;
                    clear_screen();
                }

                break;


            case DOWNLOAD_LOG_SCREEN:

                clcd_print("Downloading...", LINE1(0));
                clcd_print("Check UART", LINE2(0));

                download_log();

                __delay_ms(1000);

                screen = MENU_SCREEN;
                reset_flag = RESET_MENU_SCREEN;
                clear_screen();

                break;


            case SET_TIME_SCREEN:

                set_time_screen(key, reset_flag);
                reset_flag = RESET_NOTHING;

                if (key == SW2)
                {
                    screen = MENU_SCREEN;
                    reset_flag = RESET_MENU_SCREEN;
                    clear_screen();
                }

                break;


            case CHANGE_PASSWORD_SCREEN:

                if (change_password_screen(key, reset_flag) ==
                    RETURN_SUCCESS)
                {
                    screen = MENU_SCREEN;
                    reset_flag = RESET_MENU_SCREEN;
                    clear_screen();
                }
                else
                {
                    reset_flag = RESET_NOTHING;
                }

                break;


            default:

                screen = DASH_BOARD_SCREEN;
                reset_flag = RESET_NOTHING;
                clear_screen();

                break;
        }
    }
}