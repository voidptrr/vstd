#include <stdio.h>

#include "cstd/datastruct/binary_heap.h"

static int cmp_int_asc(const void *a, const void *b) {
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    return (lhs > rhs) - (lhs < rhs);
}

int main(void) {
    cstd_binary_heap heap;

    if (cstd_binary_heap_init(NULL, sizeof(int), cmp_int_asc) != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_binary_heap_init(NULL, ...) should return CSTD_ERR_NULL\n");
        return 1;
    }

    if (cstd_binary_heap_init(&heap, 0U, cmp_int_asc) != CSTD_ERR_RANGE) {
        fprintf(stderr, "cstd_binary_heap_init(&heap, 0, ...) should return CSTD_ERR_RANGE\n");
        return 1;
    }

    if (cstd_binary_heap_init(&heap, sizeof(int), NULL) != CSTD_ERR_NULL) {
        fprintf(stderr, "cstd_binary_heap_init(&heap, ..., NULL) should return CSTD_ERR_NULL\n");
        return 1;
    }

    if (cstd_binary_heap_init(&heap, sizeof(int), cmp_int_asc) != CSTD_OK) {
        fprintf(stderr, "cstd_binary_heap_init should return CSTD_OK\n");
        return 1;
    }

    if (!cstd_binary_heap_is_empty(&heap) || cstd_binary_heap_size(&heap) != 0U) {
        fprintf(stderr, "newly initialized heap should be empty\n");
        cstd_binary_heap_free(&heap);
        return 1;
    }

    if (cstd_binary_heap_free(&heap) != CSTD_OK) {
        fprintf(stderr, "cstd_binary_heap_free should return CSTD_OK\n");
        return 1;
    }

    return 0;
}
