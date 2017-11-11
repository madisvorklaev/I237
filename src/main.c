#include <stdio.h>
#define __ASSERT_USE_STDERR
#include <assert.h> /* The C library macro void assert(int expression) allows diagnostic information to be written to the standard error file. */
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

#define BLINK_DELAY_MS 100

void blink (const char pin) /* Function for toggleing PORTA pins */
{
    DDRA = 0x15; /* Set port A pins 0,2,4 (00010101=15h) for outputs */
    PORTA |= _BV(
                 pin); /* scan whole PORTA and shift 1 zero positions to the left (_BV=(1<<0)) */
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~_BV(pin); /* ~=bitwise NOT, toggle bit value */
    _delay_ms(BLINK_DELAY_MS);
}

/* Init error console as stderr in UART1 and print user code info */
static inline void init_errcon(void)
{
    simple_uart1_init();
    stderr = &simple_uart1_out;
    fprintf(stderr, "Version: %s built on: %s %s\n",
            FW_VERSION, __DATE__, __TIME__);
    fprintf(stderr, "avr-libc version: %s avr-gcc version: %s\n",
            __AVR_LIBC_VERSION_STRING__, __VERSION__);
}

void main (void)
{
    init_errcon();
    /* Test assert - REMOVE IN FUTURE LABS */
    char *array;
    uint32_t i = 1;
    extern int __heap_start,
           *__brkval; /* http://users.ece.utexas.edu/~valvano/arm/heap.c */
    int v;
    array = malloc( i * sizeof(char));
    assert(array);

    /* End test assert */
    while (1) {
        blink(PA0);
        blink(PA2);
        blink(PA4);
        /* Test assert - REMOVE IN FUTURE LABS */
        /*
         * Increase memory allocated for array by 100 chars
         * until we have eaten it all and print space between stack and heap.
         * That is how assert works in run-time.
         */
        array = realloc( array, (i++ * 100) * sizeof(char));
        fprintf(stderr, "%d\n",
                (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
        assert(array);
        /* End test assert */
    }
}

