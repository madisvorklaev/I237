//TODO There are most likely unnecessary includes. Clean up during lab6
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "../lib/hd44780_111/hd44780.h"
#include "../lib/andygock_avr-uart/uart.h"
#include "../lib/helius_microrl/microrl.h"
#include "hmi_msg.h"
#include "cli_microrl.h"
#include "print_helper.h"
#include "../lib/matejx_avr_lib/mfrc522.h"

#define NUM_ELEMS(x)        (sizeof(x) / sizeof((x)[0]))

void cli_print_help(const char *const *argv);
void cli_example(const char *const *argv);
void cli_print_ver(const char *const *argv);
void cli_print_ascii_tbls(const char *const *argv);
void cli_handle_number(const char *const *argv);
void cli_print_cmd_error(void);
void cli_print_cmd_arg_error(void);
void cli_rfid_read(const char *const *argv);


typedef struct cli_cmd {
    PGM_P cmd;
    PGM_P help;
    void (*func_p)();
    const uint8_t func_argc;
} cli_cmd_t;


struct Uid {
    byte size;
    byte uidByte[10];
    byte sak;
};


const char help_cmd[] PROGMEM = "help";
const char help_help[] PROGMEM = "Get help";
const char example_cmd[] PROGMEM = "example";
const char example_help[] PROGMEM =
    "Prints out all provided 3 arguments Usage: example <argument> <argument> <argument>";
const char ver_cmd[] PROGMEM = "version";
const char ver_help[] PROGMEM = "Print FW version";
const char ascii_cmd[] PROGMEM = "ascii";
const char ascii_help[] PROGMEM = "Print ASCII tables";
const char number_cmd[] PROGMEM = "number";
const char number_help[] PROGMEM =
    "Print and display matching number Usage: number <decimal number>";
const char read_cmd[] PROGMEM = "read";
const char read_help[] PROGMEM = "Reads a RFID card and prints out its info";


const cli_cmd_t cli_cmds[] = {
    {help_cmd, help_help, cli_print_help, 0},
    {ver_cmd, ver_help, cli_print_ver, 0},
    {example_cmd, example_help, cli_example, 3},
    {ascii_cmd, ascii_help, cli_print_ascii_tbls, 0},
    {number_cmd, number_help, cli_handle_number, 1},
    {read_cmd, read_help, cli_rfid_read, 0},
};


void cli_print_help(const char *const *argv)
{
    (void) argv;
    uart0_puts_p(PSTR("Implemented commands:\r\n"));

    for (uint8_t i = 0; i < NUM_ELEMS(cli_cmds); i++) {
        uart0_puts_p(cli_cmds[i].cmd);
        uart0_puts_p(PSTR(" : "));
        uart0_puts_p(cli_cmds[i].help);
        uart0_puts_p(PSTR("\r\n"));
    }
}

void cli_example(const char *const *argv)
{
    uart0_puts_p(PSTR("Command had following arguments:\r\n"));

    for (uint8_t i = 1; i < 4; i++) {
        uart0_puts(argv[i]);
        uart0_puts_p(PSTR("\r\n"));
    }
}

void cli_print_ver(const char *const *argv)
{
    (void) argv;
    uart0_puts_p(PSTR(VER_FW));
    uart0_puts_p(PSTR(VER_LIBC));
}


void cli_print_ascii_tbls(const char *const *argv)
{
    (void) argv;
    print_ascii_tbl();
    unsigned char ascii[128] = {0};

    for (unsigned char i = 0; i < sizeof(ascii); i++) {
        ascii[i] = i;
    }

    print_for_human(ascii, sizeof(ascii));
}


void cli_handle_number(const char *const *argv)
{
    (void) argv;
    int number = atoi(argv[1]);

    for (size_t i = 0; i < strlen(argv[1]); i++) {
        if (!isdigit(argv[1][i])) {
            uart0_puts_p(PSTR("Argument is not a decimal number!\r\n"));
            lcd_clr(64, 16);
            lcd_goto(LCD_ROW_2_START);
            lcd_puts_P(PSTR("Not a number!"));
            return;
        }
    }

    if (number >= 0 && number <= 9) {
        lcd_clr(64, 16);
        lcd_goto(LCD_ROW_2_START);
        uart0_puts_p(ENTERED_NUM_MESSAGE);
        uart0_puts_p((PGM_P)pgm_read_word(&(number_name[number])));
        uart0_puts_p(PSTR("\r\n"));
        lcd_puts_P((PGM_P)pgm_read_word(&(number_name[number])));
    } else {
        uart0_puts_p(OUT_OF_RANGE_NUM_MESSAGE);
        uart0_puts_p(PSTR("\r\n"));
        lcd_clr(64, 16);
        lcd_goto(LCD_ROW_2_START);
        lcd_puts_P(PSTR("Must be 0-9!"));
    }
}


void cli_print_cmd_error(void)
{
    uart0_puts_p(PSTR("Command not implemented.\r\n\tUse <help> to get help.\r\n"));
}


void cli_print_cmd_arg_error(void)
{
    uart0_puts_p(
        PSTR("Too few or too many arguments for this command\r\n\tUse <help>\r\n"));
}


void cli_rfid_read(const char *const *argv)
{
    (void) argv;
    Uid uid;
    Uid *uid_ptr = &uid;
    uart0_puts_p(PSTR("\n"));

    if (PICC_IsNewCardPresent())
    {
        uart0_puts_p(PSTR("Card selected!\r\n"));
        PICC_ReadCardSerial(uid_ptr);
        uart0_puts_p(PSTR("UID size: "));
        uart0_puts_p("0x%02X");
        uart0_putc(uid.size);
        uart0_puts_p(PSTR("\r\n"));
        uart0_puts_p(PSTR("UID sak: "));
        uart0_puts_p("0x%02X");
        uart0_putc(uid.sak);
        uart0_puts_p(PSTR("\r\n"));
        uart0_puts_p(PSTR("Card UID: "));

        for (byte i = 0; i < uid.size; i++)
        {
            uart0_puts_p("%02X");
            uart0_putc(uid.uidByte[i]);
        }
        uart0_puts_p(PSTR("\n"));
    } else
    {
        uart0_puts_p(PSTR("Unable to select card.\r\n"));
    }
}

int cli_execute(int argc, const char *const *argv)
{
    // Move cursor to new line. Then user can see what was entered.
    //FIXME Why microrl does not do it?
    uart0_puts_p(PSTR("\r\n"));

    for (uint8_t i = 0; i < NUM_ELEMS(cli_cmds); i++) {
        if (!strcmp_P(argv[0], cli_cmds[i].cmd)) {
            // Test do we have correct arguments to run command
            // Function arguments count shall be defined in struct
            if ((argc - 1) != cli_cmds[i].func_argc) {
                cli_print_cmd_arg_error();
                return 0;
            }

            // Hand argv over to function via function pointer,
            // cross fingers and hope that funcion handles it properly
            cli_cmds[i].func_p (argv);
            return 0;
        }
    }

    cli_print_cmd_error();
    return 0;
}

