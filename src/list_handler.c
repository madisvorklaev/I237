#define TRUE 1
#define FALSE 0

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


user_t *add_user(const char *name, cardnumber_t *uid, uidlength_t *length);
void delete_user(user_t *user);
entry_t *create_list(user_t *user);
void delete_list(entry_t *list_head_ptr);
void push_entry(entry_t *list_head_ptr, user_t *user);
void delete_entry(entry_t **list_head_ptr, entryid_t *rm_w_id);
void print_entry(const entry_t *entry);
void print_list(entry_t *list_head_ptr);
entryid_t get_entry_number(void);


user_t *add_user(const char *name, cardnumber_t *uid, uidlength_t *length)
{
    user_t *user_ptr = NULL;
    user_ptr = malloc(sizeof(user_t));

    if (user_ptr == NULL) {
        printf("Memory operation failed\n");
        exit(1);
    }

    user_ptr->name = malloc(strlen(name) + 1);

    if (user_ptr->name == NULL) {
        printf("Memory operation failed\n");
        free(user_ptr);
        exit(1);
    }

    strcpy(user_ptr->name, name);
    user_ptr->uid = *uid;
    user_ptr->length = *length;
    return user_ptr;
}


void delete_user(user_t *user_ptr)
{
    free(user_ptr->name);
    free(user_ptr);
}


/* That is one possible option to create first list item. Useful when handling multiple lists in same program */
entry_t *create_list(user_t *user)
{
    entry_t *list_head_ptr = malloc(sizeof(entry_t));

    if (list_head_ptr == NULL) {
        printf("Memory operation failed\n");
        exit(1);
    }

    //    printf("train_head_ptr in create_train: %p\n", train_head_ptr);
    list_head_ptr->entry_number = get_entry_number();
    list_head_ptr->user = user;
    list_head_ptr->next = NULL;
    return list_head_ptr;
}


/* Destroys train by freeing allocated memory */
void destroy_list(entry_t *list_head_ptr)
{
    entry_t *tmp_head = NULL;

    /* Start destroying from head until end is reached */
    while (list_head_ptr != NULL) {
        /* Sets temporary head */
        tmp_head = list_head_ptr;
        /* Moves head to next list element */
        list_head_ptr = list_head_ptr->next;
        /* Frees memory allocated for temporary head */
        free(tmp_head);
    }
}


/* Adds wagon to train tail */
void push_entry(entry_t *list_head_ptr, user_t *user)
{
    entry_t *current = list_head_ptr;

    while (current->next != NULL) {
        current = current->next;
    }

    current->next = malloc(sizeof(entry_t));

    if (current->next == NULL) {
        printf("Memory operation failed\n");
        exit(1);
    }

    current->next->entry_number = get_entry_number();
    current->next->user = user;
    current->next->next = NULL;
}

/* Remove specific wagon from train */
void remove_entry(entry_t **list_head_ptr, entryid_t *rm_w_id)
{
    // Note that we dereference pointer to pointer in order change head address
    // Remember? All arguments are passed by value
    // We can change only local copy or change value via pointer
    entry_t *current = *list_head_ptr;
    entry_t *prev = NULL;

    while (current != NULL) {
        if (current->entryid_t == *rm_w_id) {
            if (prev == NULL) {
                // Special condition when first item is removed
                // No need to change next value. Just point head to next item
                *list_head_ptr = current->next;
            } else {
                prev->next = current->next;
            }

            free(current);
            return;
        }

        prev = current;
        current = current->next;
    }

    uart0_puts_p(PSTR("Entry with id: ");
    uart0_putc(*rm_w_id);
    uart0_puts_p(PSTR("is not in list\r\n");
}

void print_user(const user_t *user)
{
    uart0_puts_p(PSTR("Username: "));
    uart0_putc(user->name);
    uart0_puts_p(PSTR("User's card number: "));
    uart0_putc(user->cardnumber_t);
    uart0_puts_p(PSTR("\r\n"));
}


void print_list(entry_t *list_head_ptr)
{
    entry_t *current = list_head_ptr;
    int users_in_list = 0;
    entryid_t total_users = 0;
/*    kg_t t_gargo = 0;*/
    uart0_puts_p(PSTR("Registered users:\r\n"));

    while (current != NULL) {
        uart0_puts_p(PSTR("User No. "));
        uart0_putc(++users_in_list);
        uart0_puts_p(PSTR(" Card number: "));
        uart0_putc(current->entryid_t);
        uart0_puts_p(PSTR("\r\n"));
/*        printf("Wagon No. %d ID: %d\n", ++w_in_train, current->w_u_id);*/
        total_users += current->user->entry;
        current = current->next;
    }

    if (total_users) {
        uart0_puts_p(PSTR("Total users in list ");
        uart0_putc(total_users);
        uart0_puts_p(PSTR("\r\n"));
    }
}


/* Get wagon ID. note that we hide variable from user */
entryid_t get_entry_number(void)
{
    // Unique ID for each wagon.
    // Stays in memory but is not global.
    static entryid_t entry_number = 0;
    // Return ID and increment by one for next call.
    // So ID is incremented by 1.
    return entry_number++;
}


int main(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    char s_buf[512] = {0};
    unsigned int i_buf = 0;
    user_t *user_ptr = NULL;
    entry_t *list_head_ptr = NULL;
    printf("Enter cargo (syntax: <name> <weight in kg>) >");
    scanf("%s %u", s_buf, &i_buf);
    user_ptr = add_user(s_buf, &i_buf);
    print_user(user_ptr);
    list_head_ptr = create_list(user_ptr);
    print_list(list_head_ptr);
    printf("Enter number of wagons add to train >");
    scanf("%u", & i_buf);

    for (int i = 0; i < i_buf; i++) {
        push_entry(list_head_ptr, user_ptr);
    }

    print_list(list_head_ptr);

    while (list_head_ptr != NULL) {
        printf("Enter wagon ID to be removed or -1 to exit >");
        scanf("%d", & i_buf);

        if (i_buf == -1) {
            break;
        }

        // Note that we use poiter to pointer again
        remove_entry(&list_head_ptr, &i_buf);
        print_list(list_head_ptr);
    }

    /* Watch out for memory leaks. All allocated memory shall be freed manually */
    if (list_head_ptr != NULL) {
        // Do not free memory if it was freed already earlier by removing all wagons
        destroy_list(list_head_ptr);
    }

    delete_user(user_ptr);
    return 0;
}

