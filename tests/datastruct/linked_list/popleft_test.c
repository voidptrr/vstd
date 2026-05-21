#include <stdio.h>
#include <stdlib.h>

#include "ckit/datastruct/linked_list.h"

int main(void) {
    ckit_linked_list list;
    int first = 5;
    int second = 9;
    int *out;

    ckit_linked_list_init(&list, sizeof(int), NULL);
    if (ckit_linked_list_popleft(&list) != NULL) {
        fprintf(stderr, "popleft on empty list should return NULL\n");
        ckit_linked_list_free(&list);
        return 1;
    }

    ckit_linked_list_push(&list, &first);
    ckit_linked_list_push(&list, &second);

    out = (int *)ckit_linked_list_popleft(&list);
    if (out == NULL || *out != first) {
        fprintf(stderr, "popleft should return head value\n");
        ckit_linked_list_free(&list);
        return 1;
    }
    free(out);

    out = (int *)ckit_linked_list_popleft(&list);
    if (out == NULL || *out != second) {
        fprintf(stderr, "popleft should preserve FIFO order\n");
        ckit_linked_list_free(&list);
        return 1;
    }
    free(out);

    ckit_linked_list_free(&list);
    return 0;
}
