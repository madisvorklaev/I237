#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
/*#include "uart.h"*/
#include "hmi_msg.h"
#include "print_helper.h"
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"

#define UART_BAUD           9600
#define UART_STATUS_MASK    0x00FF

#define BLINK_DELAY_MS  2000
#define LED_RED         PORTA0 // Arduino Mega digital pin 22

/* Define counter scale here NB! Uncomment only one */
#define COUNT_SECONDS         // seconds
//#define COUNT_1_10_SECONDS  // 1/10 seconds
//#define COUNT_1_100_SECONDS // 1/100 seconds

/* Un comment to get ASCII print. Otherwise bytes are sent */
#define ASCII_PRINT
#ifdef ASCII_PRINT
#include <stdlib.h> // stdlib is needed to use ltoa() - Long to ASCII
#endif /* ASCII_PRINT */

/* Global seconds counter */
volatile uint32_t counter_1;


static inline void init_led(void)
{
    /* Set pin 0 of PORTA (ARDUINO mega pin 22) for output and set low */
    DDRA |= _BV(LED_RED);
    PORTA &= ~_BV(LED_RED);
}


static inline void init_con_uart1(void)
{
    uart1_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
    uart1_puts_p(PSTR("Console started\r\n"));
}


static inline void init_counter_1(void)
{
    counter_1 = 433452345; // Set counter to random number 0x19D5F539 in HEX. Set it to 0 if you want
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1B |= _BV(WGM12); // Turn on CTC (Clear Timer on Compare)
#ifdef COUNT_SECONDS
    TCCR1B |= _BV(CS12); // fCPU/256
    OCR1A = 62549; // Note that it is actually two registers OCR5AH and OCR5AL
#endif /* COUNT_SECONDS */
#ifdef COUNT_1_10_SECONDS
    TCCR1B |= _BV(CS12); // fCPU/256
    OCR1A = 6249;
#endif /* COUNT_1_10_SECONDS */
#ifdef COUNT_1_100_SECONDS
    TCCR1B |= _BV(CS10) | _BV(CS11); // fCPU/64
    OCR1A = 2499;
#endif /* COUNT_1_100_SECONDS */
    TIMSK1 |= _BV(OCIE1A); // Output Compare A Match Interrupt Enable
}


static inline void simu_big_prog(void)
{
    /* Simulate big program with delay and toggle LED */
    PORTA ^= _BV(LED_RED);
    _delay_ms(BLINK_DELAY_MS);
}


static inline void print_counter_1(void)
{
    uint32_t counter_1_cpy = 0;
#ifdef ASCII_PRINT
    char print_buf[11] = {0x00}; // Buffer lagre enough to hold all long (uint32_t) digits
#endif /* ASCII_PRINT */
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        counter_1_cpy = counter_1;
    }
    // FIXME Help! How can we print something human readable now?
    // Where is my stdout and printf() formatting???
#ifdef ASCII_PRINT
    // Here you have something
    ltoa(counter_1_cpy, print_buf, 10);
    uart1_puts(print_buf);
    uart1_puts_p(PSTR("\r\n"));
#else
    // Send bytes. Byte 3 first.
    uart1_putc((uint8_t) (counter_1_cpy >> 24));    // Put counter byte 3
    uart1_putc((uint8_t) (counter_1_cpy >> 16));    // Put counter byte 2
    uart1_putc((uint8_t) (counter_1_cpy >> 8));     // Put counter byte 1
    uart1_putc((uint8_t) counter_1_cpy);            // Put counter byte 0
#endif /* ASCII_PRINT */
}



/*static inline void init_uart_io(void)*/
/*{*/
/*    simple_uart0_init();*/
/*    stdin = stdout = &simple_uart0_io;*/
/*    fprintf_P(stdout, PSTR("%S\n"), USERNAME);*/
/*    print_ascii_tbl(stdout);*/
/*    unsigned char ascii[128] = {0};*/

/*    for (unsigned char i = 0; i < sizeof(ascii); i++) {*/
/*        ascii[i] = i;*/
/*    }*/

/*    print_for_human(stdout, ascii, sizeof(ascii));*/
/*}*/


/* Init error console as stderr in UART1 and print user code info */
/*static inline void init_errcon(void)*/
/*{*/
/*    simple_uart1_init();*/
/*    stderr = &simple_uart1_out;*/
/*    fprintf_P(stderr, VER_FW);*/
/*    fprintf_P(stderr, VER_AVR);*/
/*}*/

void main (void)
{
/*    init_errcon(); error console on UART1*/
/*    init_uart_io();*/
/*    lcd_init();*/
/*    lcd_home();*/
/*    lcd_puts_P(USERNAME);*/
/*    int number;*/
    
    init_led();
    init_con_uart1();
    init_counter_1();
    sei(); // Enable all interrupts. Set up all interrupts before sei()!!!



    while (1) {
        simu_big_prog();
        print_counter_1();

/*        fprintf_P(stdout, PSTR("%S\n"), GET_NUM_MESSAGE);*/
/*        fscanf(stdin, "%s", &number);*/
/*        fprintf(stdout, "%s\n", &number);*/

/*        if (number >= 48 && number <= 57) {*/
/*            lcd_clr(64, 16);*/
/*            lcd_goto(LCD_ROW_2_START);*/
/*            fprintf_P(stdout, ENTERED_NUM_MESSAGE);*/
/*            fprintf_P(stdout, (PGM_P)pgm_read_word(&(numbers[number - 48])));*/
/*            fputc('\n', stdout);*/
/*            lcd_puts_P((PGM_P)pgm_read_word(&(numbers[number - 48])));*/
/*        } else {*/
/*            fprintf_P(stdout, PSTR("%S\n"), NOT_NUM_MESSAGE);*/
/*            lcd_clr(64, 16);*/
/*            lcd_goto(LCD_ROW_2_START);*/
/*            lcd_puts_P(NOT_NUM_MESSAGE);*/
/*        }*/
    }
}


/* Counter 1 ISR */
ISR(TIMER1_COMPA_vect)
{
    counter_1++;
}


