#include <stdio.h>

#include "cstd/datastruct/binary_heap.h"

static int cmp_int_asc(const void *a, const void *b) {
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    return (lhs > rhs) - (lhs < rhs);
}

int main(void) {
    cstd_binary_heap heap;
    int value = 1;

    if (cstd_binary_heap_free(NULL) != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_binary_heap_free(NULL) should return CSTD_ERR_NULL\n");
        return 1;
    }

    if (cstd_binary_heap_init(&heap, sizeof(int), cmp_int_asc) != CSTD_OK) {
        fprintf(stderr, "cstd_binary_heap_init should return CSTD_OK\n");
        return 1;
    }

    cstd_binary_heap_push(&heap, &value);

    if (cstd_binary_heap_free(&heap) != CSTD_OK) {
        fprintf(stderr, "cstd_binary_heap_free should return CSTD_OK\n");
        return 1;
    }

    if (cstd_binary_heap_size(&heap) != 0U || heap.cmp != NULL) {
        fprintf(stderr, "cstd_binary_heap_free should reset heap state\n");
        return 1;
    }

    if (cstd_binary_heap_free(&heap) != CSTD_OK) {
        fprintf(stderr, "cstd_binary_heap_free should be idempotent\n");
        return 1;
    }

    return 0;
}
