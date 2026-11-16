#ifndef CBB_H
#define CBB_H

#define DASH_BOARD_SCREEN       0x01
#define PASSWORD_SCREEN         0x02
#define MENU_SCREEN             0x03
#define VIEW_LOG_SCREEN         0x04
#define CLEAR_LOG_SCREEN        0x05
#define DOWNLOAD_LOG_SCREEN     0x06
#define SET_TIME_SCREEN         0x07
#define CHANGE_PASSWORD_SCREEN  0x08

#define RESET_PASSWORD_FLAG     0xFF
#define RESET_MENU_SCREEN       0x03
#define RESET_NOTHING           0xFE

#define TIME_OUT                0x01
#define RETURN_SUCCESS          0x02
#define RETURN_FAIL             0x03

void init_password(void);

void get_time(void);

void display_dashboard(unsigned char event[], unsigned char speed);

unsigned char check_password(unsigned char key,
                             unsigned char reset_flag);

unsigned char menu_screen(unsigned char key,
                          unsigned char reset_flag);

void log_event(unsigned char event[],
               unsigned char speed);

void view_log(unsigned char key,
              unsigned char reset_flag);

void clear_log_screen(unsigned char key,
                      unsigned char reset_flag);

void download_log(void);

void set_time_screen(unsigned char key,
                     unsigned char reset_flag);

unsigned char change_password_screen(unsigned char key,
                                     unsigned char reset_flag);

#endif