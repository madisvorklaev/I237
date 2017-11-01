#include <avr/io.h>
#include <util/delay.h>

#define BLINK_DELAY_MS 500

void blink (const char pin) /* Function for toggleing PORTA pins */
{
    DDRA = 0x15; /* Set port A pins 0,2,4 (00010101=15h) for outputs */
    PORTA |= _BV(pin); /*scan whole PORTA and shift 1 zero positions to the left (_BV=(1<<0)) */
    _delay_ms(BLINK_DELAY_MS);
    PORTA &= ~_BV(pin); /* ~=bitwise NOT, toggle bit value */
    _delay_ms(BLINK_DELAY_MS);
}

void main (void)
{
    while (1) {
        blink(PA2);
        blink(PA2);
        blink(PA2);
    }
}

