#include <stdio.h>
#define __ASSERT_USE_STDERR
#include <assert.h> /* The C library macro void assert(int expression) allows diagnostic information to be written to the standard error file. */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "uart.h"
#include "hmi_msg.h"
#include "print_helper.h"
#include "../lib/hd44780_111/hd44780.h"

#define BLINK_DELAY_MS 100

void blink (const char pin) /* Function for toggleing PORTA pins */
{
    DDRA = 0x15; /* Set port A pins 0,2,4 (00010101=15h) for outputs */
    PORTA |= _BV(pin); /* scan whole PORTA and shift 1 zero positions to the left (_BV=(1<<0)) */
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~_BV(pin); /* ~=bitwise NOT, toggle bit value */
    _delay_ms(BLINK_DELAY_MS);
}
static inline void init_uart_io(void)
{
    simple_uart0_init();
    stdin = stdout = &simple_uart0_io;
    fprintf(stdout, "%s\n", USERNAME);
    print_ascii_tbl(stdout);
    unsigned char ascii[128] = {0};
    for (unsigned char i = 0; i < sizeof(ascii); i++) {
        ascii[i] = i;
    }
    print_for_human(stdout, ascii, sizeof(ascii));
}

/* Init error console as stderr in UART1 and print user code info */
static inline void init_errcon(void)
{
    simple_uart1_init();
    stderr = &simple_uart1_out;
/*    fprintf(stderr, "Version: %s built on: %s %s\n",*/
/*            FW_VERSION, __DATE__, __TIME__);*/
/*    fprintf(stderr, "avr-libc version: %s avr-gcc version: %s\n",*/
/*            __AVR_LIBC_VERSION_STRING__, __VERSION__);*/
    fprintf(stderr, VER_FW, FW_VERSION, __DATE__, __TIME__ "\n");
    fprintf(stderr, VER_LIBC, __AVR_LIBC_VERSION_STRING__ " " VER_GCC, __VERSION__ "\n");    
}

void main (void)
{
    init_errcon(); /*error console on UART1*/
    init_uart_io();
    lcd_init();
    lcd_home();
    lcd_puts(USERNAME);
    int number;

    while (1) {
        blink(PA0);
        blink(PA2);
        blink(PA4);
        printf("Enter number >");
        fscanf(stdin, "%s", &number);
        fprintf(stdout, "%s\n", &number);
        if (number >= 48 && number <= 57) {
            lcd_clr(64,16);
            lcd_goto(LCD_ROW_2_START);
            fprintf(stdout, "%s\n", lookup[number-48]);
            lcd_puts(lookup[number-48]);
            }
        else {
            fprintf(stdout, "Enter number between 0 and 9!\n");
            lcd_clr(64,16);
            lcd_goto(LCD_ROW_2_START);
            lcd_puts("Enter number between 0 and 9!");
        }
        }
}


