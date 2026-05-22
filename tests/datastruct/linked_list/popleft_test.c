#include <stdio.h>

#include "ckit/datastruct/linked_list.h"

typedef struct test_item {
    int value;
    ckit_linked_list_node node;
} test_item;

int main(void) {
    ckit_linked_list *list;
    test_item first = {.value = 5};
    test_item second = {.value = 9};

    ckit_linked_list_node_init(&first.node);
    ckit_linked_list_node_init(&second.node);

    list = ckit_linked_list_init(NULL);
    if (ckit_linked_list_popleft(list) != NULL) {
        fprintf(stderr, "popleft on empty list should return NULL\n");
        ckit_linked_list_free(list);
        return 1;
    }

    ckit_linked_list_push(list, &first.node);
    ckit_linked_list_push(list, &second.node);

    ckit_linked_list_node *out_node = ckit_linked_list_popleft(list);
    if (out_node == NULL) {
        fprintf(stderr, "popleft should return head node\n");
        ckit_linked_list_free(list);
        return 1;
    }

    test_item *out = ckit_container_of(out_node, test_item, node);
    if (out->value != first.value) {
        fprintf(stderr, "popleft should return head value\n");
        ckit_linked_list_free(list);
        return 1;
    }

    out_node = ckit_linked_list_popleft(list);
    if (out_node == NULL) {
        fprintf(stderr, "popleft should return second node\n");
        ckit_linked_list_free(list);
        return 1;
    }

    out = ckit_container_of(out_node, test_item, node);
    if (out->value != second.value) {
        fprintf(stderr, "popleft should preserve FIFO order\n");
        ckit_linked_list_free(list);
        return 1;
    }

    ckit_linked_list_free(list);
    return 0;
}
