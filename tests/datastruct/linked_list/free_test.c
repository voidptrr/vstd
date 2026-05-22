#include <stdio.h>

#include "ckit/datastruct/linked_list.h"

typedef struct test_item {
    int value;
    ckit_linked_list_node node;
} test_item;

int main(void) {
    ckit_linked_list *list;
    test_item item = {.value = 1};

    list = ckit_linked_list_init(NULL);
    ckit_linked_list_push(list, &item.node);
    ckit_linked_list_free(list);

    return 0;
}
