#ifndef _RFID_H_
#define _RFID_H_

typedef struct card {
    uint8_t uid_size;
    uint8_t uid[10];
    char *user;
    struct card *next;
} card_t;

extern card_t *head;
extern card_t* rfid_find_card(const card_t *card);
extern void rfid_add_card(const card_t *card);
extern void rfid_list_cards(void);
extern void rfid_remove_card_by_user(const char *user);

#endif /* _RFID_H_ */

