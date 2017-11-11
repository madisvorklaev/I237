#include <avr/io.h>
#include <stdio.h>

#ifndef BAUD
#define BAUD 9600
#endif
#include <util/setbaud.h>

/* http://www.cs.mun.ca/~rod/Winter2007/4723/notes/serial/serial.html */
/* http://www.atmel.com/Images/Atmel-2549-8-bit-AVR-Microcontroller-ATmega640-1280-1281-2560-2561_datasheet.pdf */

void simple_uart0_init(void) /* initialize UART0 and set baud rate */
{
    UBRR0H = UBRRH_VALUE; /* Uart Baud Rate Register is used to set the clock for the UART shift registers */
    UBRR0L = UBRRL_VALUE; /* The UBRRnH contains the four most significant bits, and the UBRRnL contains the eight least significant bits of the USART baud rate. */
#if USE_2X
    UCSR0A |= _BV(
                  U2X0); /* The transfer rate can be doubled by setting the U2Xn bit in UCSRnA register */
#else
    UCSR0A &= ~(_BV(U2X0));
#endif
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* UCSZ=UsartCharacterSiZe, 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

void simple_uart1_init()
{
    UBRR1H = UBRRH_VALUE;
    UBRR1L = UBRRL_VALUE;
#if USE_2X
    UCSR1A |= _BV(U2X1);
#else
    UCSR1A &= ~(_BV(U2X1));
#endif
    UCSR1C = _BV(UCSZ11) | _BV(UCSZ10);
    UCSR1B = _BV(TXEN1);   /* Enable TX only (STDERR) */
}

int simple_uart0_putchar(char c, FILE *stream) /* output to UART0 */
{
    (void) stream;

    if (c == '\n') { /* newline */
        simple_uart0_putchar('\r', stream); /* carriage return */
    }

    loop_until_bit_is_set(UCSR0A, UDRE0); /* UDRE=Usart Data Register Empty */
    UDR0 = c; /* USART data register */
    return 0;
}

int simple_uart1_putchar(char c, FILE *stream) /* output to UART1 */
{
    (void) stream;

    if (c == '\n') {
        simple_uart1_putchar('\r', stream);
    }

    loop_until_bit_is_set(UCSR1A, UDRE1);
    UDR1 = c; /* USART data register */
    return 0;
}

int simple_uart0_getchar(FILE *stream) /* input from UART0 */
{
    (void) stream;
    loop_until_bit_is_set(UCSR0A, RXC0); /* RXCn=RX Complete handler */
    return UDR0; /* TX/RX buffer */
}
