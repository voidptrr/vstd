#include <stdio.h>

#include "ckit/datastruct/linked_list.h"

int main(void) {
    ckit_linked_list *list;
    int first = 1;
    int second = 2;

    list = ckit_linked_list_init(sizeof(int), NULL);
    ckit_linked_list_pushfront(list, &first);
    ckit_linked_list_pushfront(list, &second);

    int *out_second = ckit_linked_list_popleft(list);
    int *out_first = ckit_linked_list_popleft(list);

    if (ckit_linked_list_size(list) != 0U || out_second == NULL || out_first == NULL ||
        *out_second != second || *out_first != first) {
        fprintf(stderr, "pushfront should prepend\n");
        ckit_linked_list_free(list);
        return 1;
    }

    ckit_dealloc(NULL, out_second);
    ckit_dealloc(NULL, out_first);
    ckit_linked_list_free(list);
    return 0;
}
