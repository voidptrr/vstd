#include <stdio.h>

#include "ckit/datastruct/linked_list.h"

int main(void) {
    ckit_linked_list list;
    int first = 1;
    int second = 2;

    ckit_linked_list_init(&list, sizeof(int), NULL);
    ckit_linked_list_pushfront(&list, &first);
    ckit_linked_list_pushfront(&list, &second);

    if (ckit_linked_list_size(&list) != 2U || *(int *)list.head->data != second ||
        *(int *)list.tail->data != first) {
        fprintf(stderr, "pushfront should prepend\n");
        ckit_linked_list_free(&list);
        return 1;
    }

    ckit_linked_list_free(&list);
    return 0;
}
