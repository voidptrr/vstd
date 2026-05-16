#include <stdio.h>

#include "cstd/datastruct/linked_list.h"

static int test_linked_list_popleft_null_pointers(void) {
    cstd_linked_list list;
    int out = 0;

    if (cstd_linked_list_init(&list, sizeof(int)) != CSTD_OK) {
        fprintf(stderr, "cstd_linked_list_init should return CSTD_OK\n");
        return 1;
    }

    if (cstd_linked_list_popleft(NULL, &out) != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_linked_list_popleft(NULL, &out) should return CSTD_ERR_NULL\n");
        cstd_linked_list_free(&list);
        return 1;
    }

    if (cstd_linked_list_popleft(&list, NULL) != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_linked_list_popleft(&list, NULL) should return CSTD_ERR_NULL\n");
        cstd_linked_list_free(&list);
        return 1;
    }

    cstd_linked_list_free(&list);
    return 0;
}

static int test_linked_list_popleft_empty(void) {
    cstd_linked_list list;
    int out = 0;

    if (cstd_linked_list_init(&list, sizeof(int)) != CSTD_OK) {
        fprintf(stderr, "cstd_linked_list_init should return CSTD_OK\n");
        return 1;
    }

    if (cstd_linked_list_popleft(&list, &out) != CSTD_ERR_EMPTY) {
        fprintf(stderr, "cstd_linked_list_popleft on empty list should return CSTD_ERR_EMPTY\n");
        cstd_linked_list_free(&list);
        return 1;
    }

    cstd_linked_list_free(&list);
    return 0;
}

static int test_linked_list_popleft_fifo_behavior(void) {
    cstd_linked_list list;
    int first = 5;
    int second = 9;
    int out = 0;

    if (cstd_linked_list_init(&list, sizeof(int)) != CSTD_OK) {
        fprintf(stderr, "cstd_linked_list_init should return CSTD_OK\n");
        return 1;
    }

    if (cstd_linked_list_push(&list, &first) != CSTD_OK ||
        cstd_linked_list_push(&list, &second) != CSTD_OK) {
        fprintf(stderr, "cstd_linked_list_push should return CSTD_OK\n");
        cstd_linked_list_free(&list);
        return 1;
    }

    if (cstd_linked_list_popleft(&list, &out) != CSTD_OK || out != first) {
        fprintf(stderr, "first cstd_linked_list_popleft should return oldest value\n");
        cstd_linked_list_free(&list);
        return 1;
    }

    if (cstd_linked_list_popleft(&list, &out) != CSTD_OK || out != second) {
        fprintf(stderr, "second cstd_linked_list_popleft should return second value\n");
        cstd_linked_list_free(&list);
        return 1;
    }

    if (cstd_linked_list_size(&list) != 0 || list.head != NULL || list.tail != NULL) {
        fprintf(stderr, "cstd_linked_list_popleft should reset head/tail on empty\n");
        cstd_linked_list_free(&list);
        return 1;
    }

    cstd_linked_list_free(&list);
    return 0;
}

int main(void) {
    if (test_linked_list_popleft_null_pointers() != 0) {
        return 1;
    }

    if (test_linked_list_popleft_empty() != 0) {
        return 1;
    }

    if (test_linked_list_popleft_fifo_behavior() != 0) {
        return 1;
    }

    return 0;
}
