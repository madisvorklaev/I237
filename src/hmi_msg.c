#include <avr/pgmspace.h>
#ifndef HMI_MSG /* prevent the header file to be included more than once, see "wrapper #ifndef" */
#define HMI_MSG

const char USERNAME[] PROGMEM = "Madis Vorklaev";
const char VER_FW[] PROGMEM =
    "Version: "FW_VERSION" built on: "__DATE__" "__TIME__"\n";
const char VER_AVR[] PROGMEM =
    "avr-libc version: "__AVR_LIBC_VERSION_STRING__" avr-gcc version: "__VERSION__"\n";
const char GET_NUM_MESSAGE[] PROGMEM = "Enter number >";
const char NOT_NUM_MESSAGE[] PROGMEM = "Enter number between 0 and 9!";
const char ENTERED_NUM_MESSAGE[] PROGMEM = "You entered number ";

const char n1[] PROGMEM = "zero";
const char n2[] PROGMEM = "one";
const char n3[] PROGMEM = "two";
const char n4[] PROGMEM = "three";
const char n5[] PROGMEM = "four";
const char n6[] PROGMEM = "five";
const char n7[] PROGMEM = "six";
const char n8[] PROGMEM = "seven";
const char n9[] PROGMEM = "eight";
const char n10[] PROGMEM = "nine";

PGM_P const numbers[] PROGMEM = {
    n1,
    n2,
    n3,
    n4,
    n5,
    n6,
    n7,
    n8,
    n9,
    n10
};
#endif /* HMI_MSG */

