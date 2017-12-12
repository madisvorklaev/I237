#ifndef HMI_MSG_H_
#define HMI_MSG_H_

#define VER_FW   "Version: " FW_VERSION " built on: " __DATE__ " " __TIME__ "\r\n"
#define VER_LIBC "avr-libc version: " __AVR_LIBC_VERSION_STRING__ " avr-gcc version: " __VERSION__ "\r\n"

extern const char USERNAME[];
extern PGM_P const number_name[];
extern const char GET_NUM_MESSAGE[];
extern const char ENTERED_NUM_MESSAGE[];
extern const char NOT_NUM_MESSAGE[];
#endif /* HMI_MSG_H */

