#ifndef CLI_MICRORL_H
#define CLI_MICRORL_H
/* Execute callback */
int cli_execute(int argc, const char *const *argv);
void rfid_process_card(void);
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
void cli_mem_stat(const char *const *argv);
void rfid_process_card(void);
void rfid_handle_door_and_disp(void);
#endif /* CLI_MICRORL_H */

