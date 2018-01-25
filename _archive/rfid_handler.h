#ifndef RFID_HANDLER_H
#define RFID_HANDLER_H

//void rfid_read(const char *const *argv);
//void rfid_card_add(const char *const *argv);
//void rfid_card_remove(const char *const *argv);
//void rfid_card_print_list(const char *const *argv);
//void rfid_process_card(void);
//void rfid_handle_door_and_disp(void);
void print_list(node_t * head);
void push(node_t ** head, int val);
int pop(node_t ** head);
int remove_by_index(node_t ** head, int n);

#endif /* RFID_HANDLER_H */

