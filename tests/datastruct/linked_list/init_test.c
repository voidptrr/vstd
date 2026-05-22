#include <stdio.h>

#include "ckit/datastruct/linked_list.h"

int main(void) {
    ckit_linked_list *list;
    list = ckit_linked_list_init(NULL);

    if (ckit_linked_list_size(list) != 0U || !ckit_linked_list_is_empty(list)) {
        fprintf(stderr, "linked list should initialize empty\n");
        ckit_linked_list_free(list);
        return 1;
    }

    ckit_linked_list_free(list);
    return 0;
}
