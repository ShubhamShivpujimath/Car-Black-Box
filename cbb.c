#include "main.h"

#include "cbb.h"
#include "clcd.h"
#include "ds1307.h"
#include "eeprom.h"
#include "digital_keypad.h"
#include "timers.h"
#include "uart.h"

static unsigned char time[6];
static unsigned char log_data[10];
static unsigned char log_pos = 0;

#define PASSWORD_EEPROM_ADDR    100

static unsigned char password[4] =
{
    SW1, SW2, SW1, SW2
};

static const unsigned char *menu[] =
{
    (const unsigned char *)"View Log",
    (const unsigned char *)"Clear Log",
    (const unsigned char *)"Download Log",
    (const unsigned char *)"Set Time",
    (const unsigned char *)"Change Pass"
};

static unsigned char bcd_to_dec(unsigned char bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static unsigned char dec_to_bcd(unsigned char dec)
{
    return (unsigned char)(((dec / 10) << 4) | (dec % 10));
}

void init_password(void)
{
    unsigned char saved[4];
    unsigned char i;

    ext_eeprom_24C02_str_read(PASSWORD_EEPROM_ADDR, saved, 4);

  
    if (saved[0] != 0xFF)
    {
        for (i = 0; i < 4; i++)
            password[i] = saved[i];
    }
}

void get_time(void)
{
    unsigned char h;
    unsigned char m;
    unsigned char s;

    h = read_ds1307(HOUR_ADDR) & 0x3F;
    m = read_ds1307(MIN_ADDR) & 0x7F;
    s = read_ds1307(SEC_ADDR) & 0x7F;

    time[0] = ((h >> 4) & 0x03) + '0';
    time[1] = (h & 0x0F) + '0';
    time[2] = ((m >> 4) & 0x07) + '0';
    time[3] = (m & 0x0F) + '0';
    time[4] = ((s >> 4) & 0x07) + '0';
    time[5] = (s & 0x0F) + '0';
}

void display_dashboard(unsigned char event[], unsigned char speed)
{
    get_time();

    clcd_print("TIME     EV   SP", LINE1(0));

    clcd_putch(time[0], LINE2(0));
    clcd_putch(time[1], LINE2(1));
    clcd_putch(':', LINE2(2));
    clcd_putch(time[2], LINE2(3));
    clcd_putch(time[3], LINE2(4));
    clcd_putch(':', LINE2(5));
    clcd_putch(time[4], LINE2(6));
    clcd_putch(time[5], LINE2(7));

    clcd_putch(event[0], LINE2(9));
    clcd_putch(event[1], LINE2(10));

    clcd_putch((speed / 10) + '0', LINE2(14));
    clcd_putch((speed % 10) + '0', LINE2(15));
}

unsigned char check_password(unsigned char key,
                             unsigned char reset_flag)
{
    static unsigned char i;
    static unsigned char wait_for_release;

    if (reset_flag == RESET_PASSWORD_FLAG)
    {
        i = 0;
        wait_for_release = 1;

        password_timeout_flag = 0;

        clear_screen();
        clcd_print("Enter Password", LINE1(0));

        start_password_timer();

        return 0;
    }

    if (wait_for_release)
    {
        if (key == ALL_RELEASED)
            wait_for_release = 0;

        return 0;
    }

    if (password_timeout_flag)
    {
        i = 0;
        stop_password_timer();

        clear_screen();
        clcd_print("Password Timeout", LINE1(0));

        __delay_ms(1000);

        return TIME_OUT;
    }

    if (key != ALL_RELEASED)
    {
        if (key == password[i])
        {
            clcd_putch('*', LINE2(6 + i));
            i++;

            if (i == 4)
            {
                i = 0;
                stop_password_timer();

                clear_screen();
                clcd_print("Access Granted", LINE1(0));

                __delay_ms(700);

                return RETURN_SUCCESS;
            }
        }
        else
        {
            i = 0;
            stop_password_timer();

            clear_screen();
            clcd_print("Wrong Password", LINE1(0));

            __delay_ms(700);

            return RETURN_FAIL;
        }
    }

    return 0;
}

unsigned char menu_screen(unsigned char key,
                          unsigned char reset_flag)
{
    static unsigned char selected;
    static unsigned char top;
    static unsigned char update;

    if (reset_flag == RESET_MENU_SCREEN)
    {
        selected = 0;
        top = 0;
        update = 1;
    }

    if (key == SW4 && selected > 0)
    {
        selected--;

        if (selected < top)
            top--;

        update = 1;
    }
    else if (key == SW5 && selected < 4)
    {
        selected++;

        if (selected > (top + 1))
            top++;

        update = 1;
    }

    if (update)
    {
        clear_screen();

        if (selected == top)
            clcd_putch('*', LINE1(0));
        else
            clcd_putch('*', LINE2(0));

        clcd_print(menu[top], LINE1(2));
        clcd_print(menu[top + 1], LINE2(2));

        update = 0;
    }

    return selected;
}

void log_event(unsigned char event[], unsigned char speed)
{
    unsigned char addr;

    get_time();

    log_data[0] = time[0];
    log_data[1] = time[1];
    log_data[2] = time[2];
    log_data[3] = time[3];
    log_data[4] = time[4];
    log_data[5] = time[5];

    log_data[6] = event[0];
    log_data[7] = event[1];

    log_data[8] = (speed / 10) + '0';
    log_data[9] = (speed % 10) + '0';

    addr = log_pos * 10;

    ext_eeprom_24C02_str_write(addr, log_data, 10);

    log_pos++;

    if (log_pos >= 10)
        log_pos = 0;
}

void view_log(unsigned char key,
              unsigned char reset_flag)
{
    static unsigned char current;
    static unsigned char update;
    unsigned char buf[10];
    unsigned char addr;

    if (reset_flag == RESET_MENU_SCREEN)
    {
        current = 0;
        update = 1;
    }

    if (key == SW4 && current > 0)
    {
        current--;
        update = 1;
    }
    else if (key == SW5 && current < 9)
    {
        current++;
        update = 1;
    }

    if (!update)
        return;

    addr = current * 10;

    ext_eeprom_24C02_str_read(addr, buf, 10);

    clear_screen();
    clcd_print("LOG TIME EV SP  ", LINE1(0));

    if (buf[0] == 0xFF || buf[0] == 0x00)
    {
        clcd_print("No Log Available", LINE2(0));
        update = 0;
        return;
    }

    clcd_putch(buf[0], LINE2(0));
    clcd_putch(buf[1], LINE2(1));
    clcd_putch(':', LINE2(2));
    clcd_putch(buf[2], LINE2(3));
    clcd_putch(buf[3], LINE2(4));
    clcd_putch(':', LINE2(5));
    clcd_putch(buf[4], LINE2(6));
    clcd_putch(buf[5], LINE2(7));

    clcd_putch(buf[6], LINE2(10));
    clcd_putch(buf[7], LINE2(11));

    clcd_putch(buf[8], LINE2(14));
    clcd_putch(buf[9], LINE2(15));

    update = 0;
}

void clear_log_screen(unsigned char key,
                      unsigned char reset_flag)
{
    unsigned char i;

    if (reset_flag == RESET_MENU_SCREEN)
    {
        clear_screen();
        clcd_print("Clear Logs?", LINE1(0));
        clcd_print("SW1:Y SW2:N", LINE2(0));
    }

    if (key == SW1)
    {
        for (i = 0; i < 100; i++)
            ext_eeprom_24C02_byte_write(i, 0xFF);

        log_pos = 0;

        clear_screen();
        clcd_print("Logs Cleared", LINE1(0));
        __delay_ms(700);
    }
}

void download_log(void)
{
    unsigned char i;
    unsigned char buf[10];

    uart_print((const unsigned char *)
               "\r\n--- CAR BLACK BOX LOG ---\r\n");

    for (i = 0; i < 10; i++)
    {
        ext_eeprom_24C02_str_read(i * 10, buf, 10);

        if (buf[0] != 0xFF && buf[0] != 0x00)
        {
            uart_putch(buf[0]);
            uart_putch(buf[1]);
            uart_putch(':');
            uart_putch(buf[2]);
            uart_putch(buf[3]);
            uart_putch(':');
            uart_putch(buf[4]);
            uart_putch(buf[5]);

            uart_print((const unsigned char *)"  ");

            uart_putch(buf[6]);
            uart_putch(buf[7]);

            uart_print((const unsigned char *)"  ");

            uart_putch(buf[8]);
            uart_putch(buf[9]);

            uart_print((const unsigned char *)"\r\n");
        }
    }

    uart_print((const unsigned char *)"--- END ---\r\n");
}

void set_time_screen(unsigned char key,
                     unsigned char reset_flag)
{
    static unsigned char field;
    static unsigned char hour;
    static unsigned char minute;
    static unsigned char update;

    if (reset_flag == RESET_MENU_SCREEN)
    {
        hour = bcd_to_dec(read_ds1307(HOUR_ADDR) & 0x3F);
        minute = bcd_to_dec(read_ds1307(MIN_ADDR) & 0x7F);
        field = 0;
        update = 1;
    }

    if (key == SW4)
    {
        if (field == 0)
        {
            hour++;

            if (hour > 23)
                hour = 0;
        }
        else
        {
            minute++;

            if (minute > 59)
                minute = 0;
        }

        update = 1;
    }
    else if (key == SW5)
    {
        if (field == 0)
        {
            if (hour == 0)
                hour = 23;
            else
                hour--;
        }
        else
        {
            if (minute == 0)
                minute = 59;
            else
                minute--;
        }

        update = 1;
    }
    else if (key == SW6)
    {
        field ^= 1;
        update = 1;
    }
    else if (key == SW1)
    {
        write_ds1307(HOUR_ADDR, dec_to_bcd(hour));
        write_ds1307(MIN_ADDR, dec_to_bcd(minute));
        write_ds1307(SEC_ADDR, 0x00);

        update = 1;
    }

    if (update)
    {
        clear_screen();

        clcd_print("SET TIME", LINE1(0));

        clcd_putch((hour / 10) + '0', LINE2(0));
        clcd_putch((hour % 10) + '0', LINE2(1));
        clcd_putch(':', LINE2(2));
        clcd_putch((minute / 10) + '0', LINE2(3));
        clcd_putch((minute % 10) + '0', LINE2(4));

        if (field == 0)
            clcd_print("HOUR", LINE2(7));
        else
            clcd_print("MIN ", LINE2(7));

        clcd_print("SW1=SAVE", LINE2(8));

        update = 0;
    }
}

unsigned char change_password_screen(unsigned char key,
                                     unsigned char reset_flag)
{
    static unsigned char stage;
    static unsigned char i;
    static unsigned char new_pw[4];
    static unsigned char confirm_pw[4];
    unsigned char match;
    unsigned char j;

    if (reset_flag == RESET_MENU_SCREEN)
    {
        stage = 0;
        i = 0;

        clear_screen();
        clcd_print("New Password", LINE1(0));

        return 0;
    }

    if (stage == 2)
        return RETURN_SUCCESS;

    if (key == ALL_RELEASED)
        return 0;

    if (stage == 0)
    {
        new_pw[i] = key;
        clcd_putch('*', LINE2(i));
        i++;

        if (i == 4)
        {
            i = 0;
            stage = 1;

            clear_screen();
            clcd_print("Re-enter Pass", LINE1(0));
        }
    }
    else if (stage == 1)
    {
        confirm_pw[i] = key;
        clcd_putch('*', LINE2(i));
        i++;

        if (i == 4)
        {
            i = 0;

            match = 1;

            for (j = 0; j < 4; j++)
            {
                if (new_pw[j] != confirm_pw[j])
                    match = 0;
            }

            if (match)
            {
                for (j = 0; j < 4; j++)
                    password[j] = new_pw[j];

                ext_eeprom_24C02_str_write(PASSWORD_EEPROM_ADDR,
                                           password, 4);

                clear_screen();
                clcd_print("Password Changed", LINE1(0));
                __delay_ms(700);

                stage = 2;

                return RETURN_SUCCESS;
            }
            else
            {
                clear_screen();
                clcd_print("Mismatch! Retry", LINE1(0));
                __delay_ms(700);

                clear_screen();
                clcd_print("New Password", LINE1(0));

                stage = 0;
            }
        }
    }

    return 0;
}