#include <stdio.h>

#include "cstd/datastruct/binary_heap.h"

static int cmp_int_asc(const void *a, const void *b) {
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    return (lhs > rhs) - (lhs < rhs);
}

int main(void) {
    cstd_binary_heap heap;
    int value = 5;
    int top = 0;

    if (cstd_binary_heap_push(NULL, &value) != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_binary_heap_push(NULL, &value) should return CSTD_ERR_NULL\n");
        return 1;
    }

    if (cstd_binary_heap_init(&heap, sizeof(int), cmp_int_asc) != CSTD_OK) {
        fprintf(stderr, "cstd_binary_heap_init should return CSTD_OK\n");
        return 1;
    }

    if (cstd_binary_heap_push(&heap, NULL) != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_binary_heap_push(&heap, NULL) should return CSTD_ERR_NULL\n");
        cstd_binary_heap_free(&heap);
        return 1;
    }

    int values[] = {5, 2, 8, 1, 7};
    for (size_t i = 0; i < 5; i++) {
        if (cstd_binary_heap_push(&heap, &values[i]) != CSTD_OK) {
            fprintf(stderr, "cstd_binary_heap_push should return CSTD_OK\n");
            cstd_binary_heap_free(&heap);
            return 1;
        }
    }

    if (cstd_binary_heap_size(&heap) != 5U) {
        fprintf(stderr, "heap size should be 5 after pushes\n");
        cstd_binary_heap_free(&heap);
        return 1;
    }

    if (cstd_binary_heap_peek(&heap, &top) != CSTD_OK || top != 1) {
        fprintf(stderr, "heap top should be minimum element after pushes\n");
        cstd_binary_heap_free(&heap);
        return 1;
    }

    cstd_binary_heap_free(&heap);
    return 0;
}
