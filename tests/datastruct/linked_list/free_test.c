#include <stdio.h>

#include "ckit/datastruct/linked_list.h"

int main(void) {
    ckit_linked_list *list;
    int value = 1;

    list = ckit_linked_list_init(sizeof(int), NULL);
    ckit_linked_list_push(list, &value);
    ckit_linked_list_free(list);

    return 0;
}
