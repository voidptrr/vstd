#include <stddef.h>
#include <stdio.h>

#include "cstd/datastruct/linked_list.h"

static int test_linked_list_push_null_pointers(void) {
    cstd_status status;
    cstd_linked_list list;
    int value = 1;

    status = cstd_linked_list_push(NULL, &value);
    if (status != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_linked_list_push(NULL, &value) should return CSTD_ERR_NULL\n");
        return 1;
    }

    status = cstd_linked_list_init(&list, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_linked_list_init(&list, ...) should return CSTD_OK\n");
        return 1;
    }

    status = cstd_linked_list_push(&list, NULL);
    if (status != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_linked_list_push(&list, NULL) should return CSTD_ERR_NULL\n");
        return 1;
    }

    status = cstd_linked_list_free(&list);
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_linked_list_free(&list) should return CSTD_OK\n");
        return 1;
    }

    return 0;
}

static int test_linked_list_push_single_value(void) {
    cstd_status status;
    cstd_linked_list list;
    int value = 42;

    status = cstd_linked_list_init(&list, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_linked_list_init(&list, ...) should return CSTD_OK\n");
        return 1;
    }

    status = cstd_linked_list_push(&list, &value);
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_linked_list_push(&list, &value) should return CSTD_OK\n");
        return 1;
    }

    if (cstd_linked_list_size(&list) != 1 || list.head == NULL || list.tail == NULL ||
        list.head != list.tail) {
        fprintf(stderr, "cstd_linked_list_push should set head/tail and size for first node\n");
        cstd_linked_list_free(&list);
        return 1;
    }

    if (*(int *)list.head->data != 42) {
        fprintf(stderr, "cstd_linked_list_push should copy pushed element data\n");
        cstd_linked_list_free(&list);
        return 1;
    }

    status = cstd_linked_list_free(&list);
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_linked_list_free(&list) should return CSTD_OK\n");
        return 1;
    }

    return 0;
}

static int test_linked_list_push_appends_and_copies(void) {
    cstd_status status;
    cstd_linked_list list;
    int first = 7;
    int second = 11;

    status = cstd_linked_list_init(&list, sizeof(int));
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_linked_list_init(&list, ...) should return CSTD_OK\n");
        return 1;
    }

    status = cstd_linked_list_push(&list, &first);
    if (status != CSTD_OK) {
        fprintf(stderr, "first cstd_linked_list_push should return CSTD_OK\n");
        return 1;
    }

    first = 999;

    status = cstd_linked_list_push(&list, &second);
    if (status != CSTD_OK) {
        fprintf(stderr, "second cstd_linked_list_push should return CSTD_OK\n");
        cstd_linked_list_free(&list);
        return 1;
    }

    if (cstd_linked_list_size(&list) != 2 || list.head == NULL || list.tail == NULL ||
        list.head == list.tail) {
        fprintf(stderr, "cstd_linked_list_push should append and keep distinct head/tail\n");
        cstd_linked_list_free(&list);
        return 1;
    }

    if (*(int *)list.head->data != 7 || *(int *)list.tail->data != 11) {
        fprintf(stderr,
                "cstd_linked_list_push should preserve insertion order and copied values\n");
        cstd_linked_list_free(&list);
        return 1;
    }

    if (list.head->next != list.tail || list.tail->next != NULL) {
        fprintf(stderr, "cstd_linked_list_push should link nodes correctly\n");
        cstd_linked_list_free(&list);
        return 1;
    }

    status = cstd_linked_list_free(&list);
    if (status != CSTD_OK) {
        fprintf(stderr, "cstd_linked_list_free(&list) should return CSTD_OK\n");
        return 1;
    }

    return 0;
}

int main(void) {
    if (test_linked_list_push_null_pointers() != 0) {
        return 1;
    }

    if (test_linked_list_push_single_value() != 0) {
        return 1;
    }

    if (test_linked_list_push_appends_and_copies() != 0) {
        return 1;
    }

    return 0;
}
