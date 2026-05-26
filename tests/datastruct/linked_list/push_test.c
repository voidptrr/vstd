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
    test_item first = {.value = 7};
    test_item second = {.value = 11};

    list = ck_linked_list_init(NULL);
    ck_linked_list_push(list, &first.node);
    ck_linked_list_push(list, &second.node);

    ck_linked_list_node *out_first_node = ck_linked_list_popleft(list);
    ck_linked_list_node *out_second_node = ck_linked_list_popleft(list);
    if (out_first_node == NULL || out_second_node == NULL || ck_linked_list_size(list) != 0) {
        fprintf(stderr, "push should return two nodes\n");
        status = 1;
        goto cleanup;
    }

    test_item *out_first = CK_CONTAINER_OF(out_first_node, test_item, node);
    test_item *out_second = CK_CONTAINER_OF(out_second_node, test_item, node);
    if (out_first->value != first.value || out_second->value != second.value) {
        fprintf(stderr, "push should append and preserve order\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_linked_list_deinit(list);
    return status;
}
