#include <stdio.h>
#include <stdlib.h>

typedef struct user {
    char uid[10];
    char *name[];
    struct user *next;
} user_t;


/*void add_user(user_t ** head, user_t user) {*/
/*    user_t *new_user;*/
/*    new_user = malloc(sizeof(user_t));*/

/*    new_user->uid = uid;*/
/*    new_user->next = *head;*/
/*    *head = new_user;*/
/*}*/


void print_list(user_t *head) {
    user_t *current = head;

    while (current != NULL) {
        printf("%d\n", current->val);
        current = current->next;
    }
}

/*int pop(user_t **head) {*/
/*    int retval = -1;*/
/*    user_t *next_user = NULL;*/

/*    if (*head == NULL) {*/
/*        return -1;*/
/*    }*/

/*    next_user = (*head)->next;*/
/*    retval = (*head)->val;*/
/*    free(*head);*/
/*    *head = next_user;*/

/*    return retval;*/
/*}*/

/*int remove_by_uid(user_t **head, char uid) {*/
/*    user_t *previous, *current;*/

/*    if (*head == NULL) {*/
/*        return -1;*/
/*    }*/

/*    if ((*head)->name == name) {*/
/*        return pop(head);*/
/*    }*/

/*    previous = current = (*head)->next;*/
/*    while (current) {*/
/*        if (current->val == val) {*/
/*            previous->next = current->next;*/
/*            free(current);*/
/*            return val;*/
/*        }*/

/*        previous = current;*/
/*        current  = current->next;*/
/*    }*/
/*    return -1;*/
/*}*/

/*void delete_list(user_t *head) {*/
/*    user_t  *current = head, */
/*            *next = head;*/

/*    while (current) {*/
/*        next = current->next;*/
/*        free(current);*/
/*        current = next;*/
/*    }*/
/*}*/

int main(void) {
    user_t * test_list = malloc(sizeof(user_t));

    test_list->val = 1;
    test_list->next = malloc(sizeof(user_t));
    test_list->next->val = 2;
    test_list->next->next = malloc(sizeof(user_t));
    test_list->next->next->val = 3;
    test_list->next->next->next = malloc(sizeof(user_t));
    test_list->next->next->next->val = 4;
    test_list->next->next->next->next = NULL;

    remove_by_value(&test_list, 3);

    print_list(test_list);
    delete_list(test_list);

    return EXIT_SUCCESS;
}
