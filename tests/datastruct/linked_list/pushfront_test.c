#include <stdio.h>

#include "ckit/datastruct/linked_list.h"

typedef struct test_item {
    int value;
    ckit_linked_list_node node;
} test_item;

int main(void) {
    ckit_linked_list *list;
    test_item first = {.value = 1};
    test_item second = {.value = 2};

    ckit_linked_list_node_init(&first.node);
    ckit_linked_list_node_init(&second.node);

    list = ckit_linked_list_init(NULL);
    ckit_linked_list_pushfront(list, &first.node);
    ckit_linked_list_pushfront(list, &second.node);

    ckit_linked_list_node *out_second_node = ckit_linked_list_popleft(list);
    ckit_linked_list_node *out_first_node = ckit_linked_list_popleft(list);
    if (out_second_node == NULL || out_first_node == NULL || ckit_linked_list_size(list) != 0U) {
        fprintf(stderr, "pushfront should return two nodes\n");
        ckit_linked_list_free(list);
        return 1;
    }

    test_item *out_second = ckit_container_of(out_second_node, test_item, node);
    test_item *out_first = ckit_container_of(out_first_node, test_item, node);
    if (out_second->value != second.value || out_first->value != first.value) {
        fprintf(stderr, "pushfront should prepend\n");
        ckit_linked_list_free(list);
        return 1;
    }

    ckit_linked_list_free(list);
    return 0;
}
