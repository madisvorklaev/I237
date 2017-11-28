#include <avr/pgmspace.h>
#ifndef HMI_MSG /* prevent the header file to be included more than once, see "wrapper #ifndef" */
#define HMI_MSG

//#define USERNAME  "Madis Vorklaev"
//#define VER_FW "Version: %s built on: %s %s\n"
//#define VER_LIBC "avr-libc version: %s"
//#define VER_GCC  "avr-gcc version: %s\n"
const char USERNAME[] PROGMEM = "Madis Vorklaev";
const char VER_FW[] PROGMEM = "Version: %s built on: %s %s";
const char VER_AVR[] PROGMEM = "avr-libc version: %s avr-gcc version: %s\n";

//char *name = "Madis vVorklaev";
//char *VER_FW = ("Version: FW_VERSION built on: __DATE__ __TIME__\n");
const char n1[] PROGMEM = "Zero";
const char n2[] PROGMEM = "One";
const char n3[] PROGMEM = "Two";
const char n4[] PROGMEM = "Three";
const char n5[] PROGMEM = "Four";
const char n6[] PROGMEM = "Five";
const char n7[] PROGMEM = "Six";
const char n8[] PROGMEM = "Seven";
const char n9[] PROGMEM = "Eight";
const char n10[] PROGMEM = "Nine";

PGM_P const numbers[] PROGMEM = 
{
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
//const char *lookup[10] = {"Zero", "One","Two","Three","Four","Five","Six","Seven","Eight","Nine"};


#endif /* HMI_MSG */
