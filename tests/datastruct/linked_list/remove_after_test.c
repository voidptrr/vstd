#include <stdio.h>

#include "ckit/datastruct/linked_list.h"
#include "ckit/memory/utils.h"

typedef struct test_item {
    int value;
    ckit_linked_list_node node;
} test_item;

int main(void) {
    ckit_linked_list *list;
    test_item first = {.value = 1};
    test_item second = {.value = 2};
    test_item third = {.value = 3};

    list = ckit_linked_list_init(NULL);
    ckit_linked_list_push(list, &first.node);
    ckit_linked_list_push(list, &second.node);
    ckit_linked_list_push(list, &third.node);

    ckit_linked_list_node *removed = ckit_linked_list_remove_after(list, &first.node);
    test_item *removed_item = CKIT_CONTAINER_OF(removed, test_item, node);
    if (removed_item->value != 2 || ckit_linked_list_size(list) != 2) {
        fprintf(stderr, "remove_after should remove node after prev\n");
        ckit_linked_list_deinit(list);
        return 1;
    }

    removed = ckit_linked_list_remove_after(list, NULL);
    removed_item = CKIT_CONTAINER_OF(removed, test_item, node);
    if (removed_item->value != 1 || ckit_linked_list_head(list) != &third.node) {
        fprintf(stderr, "remove_after with null prev should remove head\n");
        ckit_linked_list_deinit(list);
        return 1;
    }

    removed = ckit_linked_list_remove_after(list, &third.node);
    if (removed != NULL || ckit_linked_list_size(list) != 1) {
        fprintf(stderr, "remove_after should return null when prev is tail\n");
        ckit_linked_list_deinit(list);
        return 1;
    }

    ckit_linked_list_deinit(list);
    return 0;
}
