#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include "hmi_msg.h"
#include "print_helper.h"
#include "../lib/helius_microrl/microrl.h"
#include "cli_microrl.h"
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"

#define UART_BAUD           9600
#define UART_STATUS_MASK    0x00FF

#define BLINK_DELAY_MS  900
#define LED_RED         PORTA0 // Arduino Mega digital pin 22
#define LED_GREEN       PORTA2 // Arduino Mega digital pin 24

/* Define counter scale here NB! Uncomment only one */
#define COUNT_SECONDS         // seconds
//#define COUNT_1_10_SECONDS  // 1/10 seconds
//#define COUNT_1_100_SECONDS // 1/100 seconds

/* Un comment to get ASCII print. Otherwise bytes are sent */
#define ASCII_PRINT
#ifdef ASCII_PRINT
#include <stdlib.h> // stdlib is needed to use ltoa() - Long to ASCII
#endif /* ASCII_PRINT */


volatile uint32_t counter_1; //Global seconds counter
uint32_t prev_time = 0; //Heartbeat time placeholder
uint32_t now = 0; //Heartbeat time placeholder

//  Create  microrl object  and pointer on  it
microrl_t rl;
microrl_t *prl = &rl;

static inline void init_leds(void)
{
    /* RGB LED board set up and off */
    DDRA |= _BV(LED_RED) | _BV(LED_GREEN);
    PORTA &= ~(_BV(LED_RED) | _BV(LED_GREEN));
}


static inline void init_con_uarts(void)
{
    uart0_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
    uart0_puts_p(PSTR("Console started\r\n"));
    uart0_puts_p(USERNAME);
    uart0_puts_p(PSTR("\r\n"));
    uart1_init(UART_BAUD_SELECT(UART_BAUD, F_CPU));
    uart1_puts_p(PSTR("Console started\r\n"));
    uart1_puts_p(USERNAME);
    uart1_puts_p(PSTR("\r\n"));
    // call init with ptr to microrl instance and print callback
    microrl_init (prl, uart0_puts);
    // set callback for execute
    microrl_set_execute_callback (prl, cli_execute);
}


static inline void init_counter_1(void)
{
    counter_1 = 0; // Set counter to 0
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


static inline void heartbeat(void)
{
#ifdef ASCII_PRINT
    char print_buf[11] = {0x00}; // Buffer lagre enough to hold all long (uint32_t) digits
#endif /* ASCII_PRINT */
    ATOMIC_BLOCK(ATOMIC_FORCEON) {
        now = counter_1;
    }

    if (prev_time != now) {
        ltoa(now, print_buf, 10);
        uart1_puts_p(PSTR("Uptime: "));
        uart1_puts(print_buf);
        uart1_puts_p(PSTR(" s\r\n"));
        PORTA ^= _BV(LED_GREEN);
        ATOMIC_BLOCK(ATOMIC_FORCEON) {
            prev_time = now;
        }
    }

    /*#else*/
    /*    // Send bytes. Byte 3 first.*/
    /*    uart0_putc((uint8_t) (counter_1_cpy >> 24));    // Put counter byte 3*/
    /*    uart0_putc((uint8_t) (counter_1_cpy >> 16));    // Put counter byte 2*/
    /*    uart0_putc((uint8_t) (counter_1_cpy >> 8));     // Put counter byte 1*/
    /*    uart0_putc((uint8_t) counter_1_cpy);            // Put counter byte 0*/
}


void main (void)
{

    lcd_init();
    lcd_home();
    lcd_puts_P(USERNAME);
    /*    int number;*/
    sei(); // Enable all interrupts. Set up all interrupts before sei()!!!
    init_leds();
    init_con_uarts();
    init_counter_1();

    while (1) {
        heartbeat();
        //CLI commands are handled in cli_execute()
        microrl_insert_char(prl, (uart0_getc() & UART_STATUS_MASK));
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


