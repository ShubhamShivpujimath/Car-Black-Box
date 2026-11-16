# Car Black Box

Embedded C project implementing a car event logging system using a PIC microcontroller.

## Features visible in the source
- Dashboard display with vehicle speed value read through the ADC
- Gear/event navigation using keypad switches
- Clutch event logging
- Password-protected menu access
- Event logging and log-management screens
- RTC interface for date/time support
- UART interface configured for serial communication
- EEPROM interface for log/password storage

## Hardware and software
- PIC microcontroller (confirm the exact device in the MPLAB project configuration)
- Character LCD (CLCD)
- ADC input / speed-sensor signal interface
- Digital keypad
- DS1307 RTC
- EEPROM
- UART serial interface
- MPLAB X IDE
- MPLAB XC8 compiler

## Source layout
- `main.c` – application flow and dashboard event handling
- `cbb.c`, `cbb.h` – car black box application functions and screen definitions
- `adc.c`, `adc.h` – ADC interface
- `clcd.c`, `clcd.h` – character LCD interface
- `digital_keypad.c`, `digital_keypad.h` – keypad interface
- `ds1307.c`, `ds1307.h`, `i2c.c`, `i2c.h` – RTC and I2C interfaces
- `eeprom.c`, `eeprom.h` – EEPROM interface
- `timers.c`, `uart.c` and related headers – timer and UART support
- `isr.c` – interrupt service routine

## Build
1. Open the `.X` project folder in MPLAB X IDE.
2. Select the matching PIC device and XC8 toolchain configured for the project.
3. Build the project.
4. Connect and verify the RTC, LCD, keypad, ADC input, EEPROM, and UART interfaces before system testing.

## Notes
The repository includes source files and MPLAB project configuration. Build outputs and generated IDE artifacts are intentionally excluded. Add circuit diagrams, simulation screenshots, and test results when available.
