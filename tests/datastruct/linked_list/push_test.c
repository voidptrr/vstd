#include <stdio.h>

#include "ckit/datastruct/linked_list.h"

int main(void) {
    ckit_linked_list *list;
    int first = 7;
    int second = 11;

    list = ckit_linked_list_init(sizeof(int), NULL);
    ckit_linked_list_push(list, &first);
    ckit_linked_list_push(list, &second);

    int *out_first = ckit_linked_list_popleft(list);
    int *out_second = ckit_linked_list_popleft(list);

    if (ckit_linked_list_size(list) != 0U || out_first == NULL || out_second == NULL ||
        *out_first != first || *out_second != second) {
        fprintf(stderr, "push should append and preserve order\n");
        ckit_linked_list_free(list);
        return 1;
    }

    ckit_dealloc(NULL, out_first);
    ckit_dealloc(NULL, out_second);
    ckit_linked_list_free(list);
    return 0;
}
