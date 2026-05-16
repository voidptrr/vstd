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
    int value = 3;

    if (cstd_binary_heap_peek(NULL, &out) != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_binary_heap_peek(NULL, &out) should return CSTD_ERR_NULL\n");
        return 1;
    }

    if (cstd_binary_heap_init(&heap, sizeof(int), cmp_int_asc) != CSTD_OK) {
        fprintf(stderr, "cstd_binary_heap_init should return CSTD_OK\n");
        return 1;
    }

    if (cstd_binary_heap_peek(&heap, NULL) != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_binary_heap_peek(&heap, NULL) should return CSTD_ERR_NULL\n");
        cstd_binary_heap_free(&heap);
        return 1;
    }

    if (cstd_binary_heap_peek(&heap, &out) != CSTD_ERR_EMPTY) {
        fprintf(stderr, "cstd_binary_heap_peek on empty heap should return CSTD_ERR_EMPTY\n");
        cstd_binary_heap_free(&heap);
        return 1;
    }

    if (cstd_binary_heap_push(&heap, &value) != CSTD_OK) {
        fprintf(stderr, "cstd_binary_heap_push should return CSTD_OK\n");
        cstd_binary_heap_free(&heap);
        return 1;
    }

    if (cstd_binary_heap_peek(&heap, &out) != CSTD_OK || out != 3) {
        fprintf(stderr, "cstd_binary_heap_peek should return top element\n");
        cstd_binary_heap_free(&heap);
        return 1;
    }

    if (cstd_binary_heap_size(&heap) != 1U) {
        fprintf(stderr, "cstd_binary_heap_peek should not change heap size\n");
        cstd_binary_heap_free(&heap);
        return 1;
    }

    cstd_binary_heap_free(&heap);
    return 0;
}
