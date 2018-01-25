typedef char cardnumber_t;
typedef unsigned int uidlength_t;
typedef unsigned int entryid_t;


typedef struct user {
    char *name;
    cardnumber_t uid;
    uidlength_t length;
} user_t;


typedef struct entry {
    entryid_t entry_number;
    user_t *user;
    struct entry *next;
} entry_t;


user_t *add_user(const char *name, cardnumber_t *uid);
void delete_user(user_t *user);
entry_t *create_list(user_t *user);
void delete_list(entry_t *list_head_ptr);
void push_entry(entry_t *list_head_ptr, user_t *user);
void delete_entry(entry_t **list_head_ptr, entryid_t *rm_w_id);
void print_entry(const entry_t *entry);
void print_list(entry_t *list_head_ptr);
entryid_t get_entry_number(void);
