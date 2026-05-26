#include <stdio.h>

#include "ckit/datastruct/linked_list.h"
#include "ckit/utils.h"

typedef struct test_item {
    int value;
    ck_linked_list_node node;
} test_item;

int main(void) {
    int status = 0;
    ck_linked_list *list;
    test_item first = {.value = 5};
    test_item second = {.value = 9};

    list = ck_linked_list_init(NULL);
    if (ck_linked_list_popleft(list) != NULL) {
        fprintf(stderr, "popleft on empty list should return NULL\n");
        status = 1;
        goto cleanup;
    }

    ck_linked_list_push(list, &first.node);
    ck_linked_list_push(list, &second.node);

    ck_linked_list_node *out_node = ck_linked_list_popleft(list);
    if (out_node == NULL) {
        fprintf(stderr, "popleft should return head node\n");
        status = 1;
        goto cleanup;
    }

    test_item *out = CK_CONTAINER_OF(out_node, test_item, node);
    if (out->value != first.value) {
        fprintf(stderr, "popleft should return head value\n");
        status = 1;
        goto cleanup;
    }

    out_node = ck_linked_list_popleft(list);
    if (out_node == NULL) {
        fprintf(stderr, "popleft should return second node\n");
        status = 1;
        goto cleanup;
    }

    out = CK_CONTAINER_OF(out_node, test_item, node);
    if (out->value != second.value) {
        fprintf(stderr, "popleft should preserve FIFO order\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_linked_list_deinit(list);
    return status;
}
