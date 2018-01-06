#ifndef HMI_MSG_H_
#define HMI_MSG_H_

#define VER_FW   "Version: " FW_VERSION " built on: " __DATE__ " " __TIME__ "\r\n"
#define VER_LIBC "avr-libc version: " __AVR_LIBC_VERSION_STRING__ " avr-gcc version: " __VERSION__ "\r\n"

//extern const char VER_FW[];
//extern const char VER_AVR[];
extern const char STARTUP_MESSAGE[];
extern const char USERNAME[];
extern const char ENTERED_NUM_MESSAGE[];
extern const char OUT_OF_RANGE_NUM_MESSAGE[];
extern PGM_P const number_name[];
#endif /* HMI_MSG_H */

