#include <avr/pgmspace.h>
#ifndef HMI_MSG /* prevent the header file to be included more than once, see "wrapper #ifndef" */
#define HMI_MSG

#define USERNAME "Madis Vorklaev"
#define VER_FW "Version: %s built on: %s %s\n"
#define VER_LIBC "avr-libc version: %s"
#define VER_GCC  "avr-gcc version: %s\n"

//char *name = "Madis vVorklaev";
//char *VER_FW = ("Version: FW_VERSION built on: __DATE__ __TIME__\n");
const char *lookup[10] = {"Zero", "One","Two","Three","Four","Five","Six","Seven","Eight","Nine"};


#endif /* HMI_MSG */
