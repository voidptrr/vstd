#include <stdio.h>

#include "ckit/datastruct/linked_list.h"

int main(void) {
    int status = 0;
    ck_linked_list *list;
    list = ck_linked_list_init(NULL);

    if (ck_linked_list_size(list) != 0) {
        fprintf(stderr, "linked list should initialize empty\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_linked_list_deinit(list);
    return status;
}
