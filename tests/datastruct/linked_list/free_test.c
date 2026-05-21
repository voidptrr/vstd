#include <stdio.h>

#include "ckit/datastruct/linked_list.h"

int main(void) {
    ckit_linked_list list;
    int value = 1;

    ckit_linked_list_init(&list, sizeof(int), NULL);
    ckit_linked_list_push(&list, &value);
    ckit_linked_list_free(&list);

    if (list.head != NULL || list.tail != NULL || ckit_linked_list_size(&list) != 0U) {
        fprintf(stderr, "free should reset list state\n");
        return 1;
    }

    return 0;
}
