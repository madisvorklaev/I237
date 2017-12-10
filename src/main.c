#include <stdio.h>
/*#include <time.h>*/
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "uart.h"
#include "hmi_msg.h"
#include "print_helper.h"
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"

#define UART_BAUD           9600
#define UART_STATUS_MASK    0x00FF

#define BLINK_DELAY_MS  2000
#define LED_RED         PORTA0 // Arduino Mega digital pin 22


static inline void init_led(void)
{
    /* Set pin 0 of PORTA (ARDUINO mega pin 22) for output and set low */
    DDRA |= _BV(LED_RED);
    PORTA &= ~_BV(LED_RED);
}


void blink (const char pin) /* Function for toggleing PORTA pins */
{
    DDRA = 0x15; /* Set port A pins 0,2,4 (00010101=15h) for outputs */
    PORTA |= _BV(
                 pin); /* scan whole PORTA and shift 1 zero positions to the left (_BV=(1<<0)) */
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~_BV(pin); /* ~=bitwise NOT, toggle bit value */
    _delay_ms(BLINK_DELAY_MS);
}



/*static inline void init_con_uart1(void)*/
/*{*/
/*    uart1_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));*/
/*    uart1_puts_p(PSTR("Console started\r\n"));*/
/*}*/


/*static inline void init_sys_timer(void)*/
/*{*/
/*    //    counter_1 = 0; // Set counter to random number 0x19D5F539 in HEX. Set it to 0 if you want*/
/*    TCCR1A = 0;*/
/*    TCCR1B = 0;*/
/*    TCCR1B |= _BV(WGM12); // Turn on CTC (Clear Timer on Compare)*/
/*    TCCR1B |= _BV(CS12); // fCPU/256*/
/*    OCR1A = 62549; // Note that it is actually two registers OCR5AH and OCR5AL*/
/*    TIMSK1 |= _BV(OCIE1A); // Output Compare A Match Interrupt Enable*/
/*}*/


static inline void init_uart_io(void)
{
    simple_uart0_init();
    stdin = stdout = &simple_uart0_io;
    fprintf_P(stdout, PSTR("%S\n"), USERNAME);
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
    fprintf_P(stderr, VER_FW);
    fprintf_P(stderr, VER_AVR);
}

void main (void)
{
    init_errcon(); /*error console on UART1*/
    init_uart_io();
    lcd_init();
    lcd_home();
    lcd_puts_P(USERNAME);
    int number;

    while (1) {
        blink(PA0);
        blink(PA2);
        blink(PA4);
        fprintf_P(stdout, PSTR("%S\n"), GET_NUM_MESSAGE);
        fscanf(stdin, "%s", &number);
        fprintf(stdout, "%s\n", &number);

        if (number >= 48 && number <= 57) {
            lcd_clr(64, 16);
            lcd_goto(LCD_ROW_2_START);
            fprintf_P(stdout, ENTERED_NUM_MESSAGE);
            fprintf_P(stdout, (PGM_P)pgm_read_word(&(numbers[number - 48])));
            fputc('\n', stdout);
            lcd_puts_P((PGM_P)pgm_read_word(&(numbers[number - 48])));
        } else {
            fprintf_P(stdout, PSTR("%S\n"), NOT_NUM_MESSAGE);
            lcd_clr(64, 16);
            lcd_goto(LCD_ROW_2_START);
            lcd_puts_P(NOT_NUM_MESSAGE);
        }
    }
}


/* Counter 1 ISR */
/*ISR(TIMER1_COMPA_vect)*/
/*{*/
/*    system_tick();*/
/*}*/

