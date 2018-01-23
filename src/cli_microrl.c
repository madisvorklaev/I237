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

typedef unsigned int id_t;

typedef struct cli_cmd {
    PGM_P cmd;
    PGM_P help;
    void (*func_p)();
    const uint8_t func_argc;
} cli_cmd_t;


typedef struct card {
    char *name;
    uint8_t *uid;
    int size;
} card_t;


typedef struct card_list {
    id_t card_id;
    card_t *card;
    struct card_list *next;
} card_list_t;

card_list_t *list_head_ptr;
card_t *card_ptr;
char print_buf[256] = {0x00};


void cli_print_help(const char *const *argv);
void cli_example(const char *const *argv);
void cli_print_ver(const char *const *argv);
void cli_print_ascii_tbls(const char *const *argv);
void cli_handle_number(const char *const *argv);
void cli_print_cmd_error(void);
void cli_print_cmd_arg_error(void);
void rfid_card_read(const char *const *argv);
void rfid_card_add(const char *const *argv);
void rfid_card_remove(const char *const *argv);
void rfid_card_print_list(const char *const *argv);
//void rfid_process_card(void);
//void rfid_handle_door_and_disp(void);
id_t get_id(void);

typedef struct {
    byte size;
    byte uidByte[10];
    byte sak;
    byte bufferATQA[10];
} current_uid;


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
const char read_help[] PROGMEM = "Read RFID card";
const char add_cmd[] PROGMEM = "add";
const char add_help[] PROGMEM = "Add RFID card";
const char print_cmd[] PROGMEM = "print";
const char print_help[] PROGMEM = "Print all RFID cards";
const char remove_cmd[] PROGMEM = "remove";
const char remove_help[] PROGMEM = "Remove RFID card. Usage: remove <RFID card number>";


const cli_cmd_t cli_cmds[] = {
    {help_cmd, help_help, cli_print_help, 0},
    {ver_cmd, ver_help, cli_print_ver, 0},
    {example_cmd, example_help, cli_example, 3},
    {ascii_cmd, ascii_help, cli_print_ascii_tbls, 0},
    {number_cmd, number_help, cli_handle_number, 1},
    {read_cmd, read_help, rfid_card_read, 0},
    {add_cmd, add_help, rfid_card_add, 3},
    {remove_cmd, remove_help, rfid_card_remove, 1},
    {print_cmd, print_help, rfid_card_print_list, 0},
    //{process_cmd, process_help, rfid_process_card, 0},
    //{disp_cmd, disp_help, rfid_handle_door_and_disp, 0},
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


card_list_t *create_list(card_t *card)
{
    card_list_t *list_head_ptr = malloc(sizeof(card_list_t));

    if (list_head_ptr == NULL) {
        uart0_puts_p(PSTR("Memory operation failed\r\n"));
        exit(1);
    }

    list_head_ptr->card_id = get_id();
    list_head_ptr->card = card;
    list_head_ptr->next = NULL;
    return list_head_ptr;
}


card_t *create_card(const char *name, const char *input_uid, const int size)
{
    card_ptr = malloc(sizeof(card_t));

    if (card_ptr == NULL) {
        uart0_puts_p(PSTR("Memory operation failed\r\n"));
        exit(1);
    }

    uint8_t *bytes = malloc((size / 2) * sizeof(uint8_t));

    if (bytes == NULL) {
        uart0_puts_p(PSTR("Memory operation failed\r\n"));
        free(bytes);
        exit(1);
    }

    tallymarker_hextobin(input_uid, bytes, size);
    card_list_t *current = list_head_ptr;

    while (current != NULL) {
        if (memcmp(current->card->uid, bytes, current->card->size) == 0) {
            uart0_puts_p(PSTR("This card is already in use.\r\n"));
            return 0;
        }

        current = current->next;
    }

    card_ptr->name = malloc(strlen(name) + 1);

    if (card_ptr->name == NULL) {
        uart0_puts_p(PSTR("Memory operation failed\r\n"));
        free(card_ptr);
        exit(1);
    }

    strcpy(card_ptr->name, name);
    card_ptr->size = size / 2;
    card_ptr->uid = bytes;
    uart0_puts_p(PSTR("Card added.\r\nCard UID: "));
    print_bytes(card_ptr->uid, card_ptr->size);
    uart0_puts_p(PSTR("  Card UID size: "));
    sprintf_P(print_buf, PSTR("%i"), card_ptr->size);
    uart0_puts(print_buf);
    uart0_puts_p(PSTR("\r\n"));
    uart0_puts_p(PSTR("Card holder name: "));
    uart0_puts(card_ptr->name);
    uart0_puts_p(PSTR("\r\n"));
    return card_ptr;
}


void push_card(card_list_t *list_head_ptr, card_t *card)
{
    card_list_t *current = list_head_ptr;

    while (current->next != NULL) {
        current = current->next;
    }

    current->next = malloc(sizeof(card_list_t));

    if (current->next == NULL) {
        printf("Memory operation failed\n");
        exit(1);
    }

    current->next->card_id = get_id();
    current->next->card = card;
    current->next->next = NULL;
}


void remove_card(const char *input_uid, card_list_t **list_head_ptr) {
    card_list_t *current = *list_head_ptr;
    card_list_t *prev = NULL;

    uint8_t *bytes = malloc((strlen(input_uid)/2) * sizeof(uint8_t));
    tallymarker_hextobin(input_uid, bytes, strlen(input_uid));

     while (current != NULL) {
        if (memcmp(current->card->uid, bytes, current->card->size) == 0) {
            if (prev == NULL) {
                *list_head_ptr = current->next;
            } else {
                prev->next = current->next;
            }
        
            uart0_puts_p(PSTR("Card with UID: "));
            uart0_puts(input_uid);
            uart0_puts_p(PSTR(" is removed."));
            uart0_puts_p(PSTR("\r\n"));

            free(current->card->name);
            free(current->card->uid);
            free(current);
            return;
        }

        prev = current;
        current = current->next;
    }

    uart0_puts_p(PSTR("Card with UID: "));
    uart0_puts(input_uid);
    uart0_puts_p(PSTR(" is not in list."));
}


id_t get_id(void)
{
    static id_t card_u_id = 0;
    return card_u_id++;
}


void rfid_card_read(const char *const *argv)
{
    (void) argv;
    Uid uid;
    byte bufferSize = sizeof(uid.bufferATQA);;
    uart0_puts_p(PSTR("\r\n"));

    if (PICC_WakeupA(uid.bufferATQA, &bufferSize) != STATUS_OK) {
        if (PICC_IsNewCardPresent()) {
            uart0_puts_p(PSTR("Card selected!\r\n"));
            PICC_ReadCardSerial(&uid);
            uart0_puts_p(PSTR("Card type: "));
            uart0_puts(PICC_GetTypeName(PICC_GetType(uid.sak)));
            uart0_puts_p(PSTR("\r\n"));
            uart0_puts_p(PSTR("Card UID: "));
            print_bytes(uid.uidByte, uid.size);
            uart0_puts_p(PSTR("  Card UID size: "));
            print_bytes(&uid.size, 1);
            uart0_puts_p(PSTR("\r\n"));
        } else {
            uart0_puts_p((PSTR("Unable to select card.\r\n")));
        }
    }
}


void rfid_card_add(const char *const *argv)
{
    (void) argv;
    int input_uid_length = strlen(argv[2]);

    if (input_uid_length > 20) {
        uart0_puts_p(PSTR("Card has to be max 10 bytes.\r\n"));
        return;
    }

    card_ptr = create_card(argv[1], argv[2], input_uid_length);

    if (list_head_ptr == 0 && card_ptr != 0) {
        list_head_ptr = create_list(card_ptr);
    } else if (card_ptr != 0) {
        push_card(list_head_ptr, card_ptr);
    } else {
        uart0_puts_p(PSTR("Adding card failed.\r\n"));
    }
}


void rfid_card_remove(const char *const *argv)
{
    if (list_head_ptr == 0) {
        uart0_puts_p(PSTR("There are no cards in the list."));
        uart0_puts_p(PSTR("\r\n"));
    }
    else {
        remove_card(argv[1], &list_head_ptr);
    }
}


void rfid_card_print_list(const char *const *argv)
{
    (void) argv;
    card_list_t *current = list_head_ptr;

    while (current != NULL) {
        uart0_puts(current->card->name);
        uart0_puts_p(PSTR("\r\n"));
        current = current->next;
    }
}
