#include <stdio.h>

#include "ckit/datastruct/linked_list.h"

typedef struct test_item {
    int value;
    ck_linked_list_node node;
} test_item;

int main(void) {
    int status = 0;
    ck_linked_list *list;
    test_item first = {.value = 1};

    list = ck_linked_list_init(NULL);
    if (ck_linked_list_head(list) != NULL) {
        fprintf(stderr, "empty list head should be null\n");
        status = 1;
        goto cleanup;
    }

    ck_linked_list_push(list, &first.node);
    if (ck_linked_list_head(list) != &first.node) {
        fprintf(stderr, "list head should return first node\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_linked_list_deinit(list);
    return status;
}
