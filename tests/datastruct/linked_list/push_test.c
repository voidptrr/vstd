#include <stdio.h>

#include "ckit/datastruct/linked_list.h"

int main(void) {
    ckit_linked_list list;
    int first = 7;
    int second = 11;

    ckit_linked_list_init(&list, sizeof(int), NULL);
    ckit_linked_list_push(&list, &first);
    ckit_linked_list_push(&list, &second);

    if (ckit_linked_list_size(&list) != 2U || *(int *)list.head->data != first ||
        *(int *)list.tail->data != second) {
        fprintf(stderr, "push should append and preserve order\n");
        ckit_linked_list_free(&list);
        return 1;
    }

    ckit_linked_list_free(&list);
    return 0;
}
