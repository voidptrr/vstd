#include <stdio.h>

#include "cstd/datastruct/binary_heap.h"

static int cmp_int_asc(const void *a, const void *b) {
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    return (lhs > rhs) - (lhs < rhs);
}

int main(void) {
    cstd_binary_heap heap;
    int out = 0;

    if (cstd_binary_heap_pop(NULL, &out) != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_binary_heap_pop(NULL, &out) should return CSTD_ERR_NULL\n");
        return 1;
    }

    if (cstd_binary_heap_init(&heap, sizeof(int), cmp_int_asc) != CSTD_OK) {
        fprintf(stderr, "cstd_binary_heap_init should return CSTD_OK\n");
        return 1;
    }

    if (cstd_binary_heap_pop(&heap, NULL) != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_binary_heap_pop(&heap, NULL) should return CSTD_ERR_NULL\n");
        cstd_binary_heap_free(&heap);
        return 1;
    }

    if (cstd_binary_heap_pop(&heap, &out) != CSTD_ERR_EMPTY) {
        fprintf(stderr, "cstd_binary_heap_pop on empty heap should return CSTD_ERR_EMPTY\n");
        cstd_binary_heap_free(&heap);
        return 1;
    }

    int values[] = {9, 4, 6, 1, 7, 2};
    for (size_t i = 0; i < 6; i++) {
        if (cstd_binary_heap_push(&heap, &values[i]) != CSTD_OK) {
            fprintf(stderr, "cstd_binary_heap_push should return CSTD_OK\n");
            cstd_binary_heap_free(&heap);
            return 1;
        }
    }

    int expected[] = {1, 2, 4, 6, 7, 9};
    for (size_t i = 0; i < 6; i++) {
        if (cstd_binary_heap_pop(&heap, &out) != CSTD_OK || out != expected[i]) {
            fprintf(stderr, "cstd_binary_heap_pop should return values in comparator order\n");
            cstd_binary_heap_free(&heap);
            return 1;
        }
    }

    if (!cstd_binary_heap_is_empty(&heap)) {
        fprintf(stderr, "heap should be empty after popping all elements\n");
        cstd_binary_heap_free(&heap);
        return 1;
    }

    cstd_binary_heap_free(&heap);
    return 0;
}
