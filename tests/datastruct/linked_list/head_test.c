#include <stdio.h>

#include "ckit/datastruct/linked_list.h"

typedef struct test_item {
    int value;
    ckit_linked_list_node node;
} test_item;

int main(void) {
    ckit_linked_list *list;
    test_item first = {.value = 1};

    list = ckit_linked_list_init(NULL);
    if (ckit_linked_list_head(list) != NULL) {
        fprintf(stderr, "empty list head should be null\n");
        ckit_linked_list_deinit(list);
        return 1;
    }

    ckit_linked_list_push(list, &first.node);
    if (ckit_linked_list_head(list) != &first.node) {
        fprintf(stderr, "list head should return first node\n");
        ckit_linked_list_deinit(list);
        return 1;
    }

    ckit_linked_list_deinit(list);
    return 0;
}
