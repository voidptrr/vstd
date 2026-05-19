#include <stdio.h>

#include "ckit/datastruct/binary_heap.h"

static int cmp_int_asc(const void *a, const void *b) {
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    return (lhs > rhs) - (lhs < rhs);
}

int main(void) {
    ckit_binary_heap heap;

    if (ckit_binary_heap_init(NULL, sizeof(int), cmp_int_asc, NULL) != CKIT_ERR_NULL) {
        fprintf(stderr, "ckit_binary_heap_init(NULL, ...) should return CKIT_ERR_NULL\n");
        return 1;
    }

    if (ckit_binary_heap_init(&heap, 0U, cmp_int_asc, NULL) != CKIT_ERR_RANGE) {
        fprintf(stderr,
                "ckit_binary_heap_init(&heap, 0, ..., NULL) should return CKIT_ERR_RANGE\n");
        return 1;
    }

    if (ckit_binary_heap_init(&heap, sizeof(int), NULL, NULL) != CKIT_ERR_NULL) {
        fprintf(stderr,
                "ckit_binary_heap_init(&heap, ..., NULL, NULL) should return CKIT_ERR_NULL\n");
        return 1;
    }

    if (ckit_binary_heap_init(&heap, sizeof(int), cmp_int_asc, NULL) != CKIT_OK) {
        fprintf(stderr, "ckit_binary_heap_init should return CKIT_OK\n");
        return 1;
    }

    if (!ckit_binary_heap_is_empty(&heap) || ckit_binary_heap_size(&heap) != 0U) {
        fprintf(stderr, "newly initialized heap should be empty\n");
        ckit_binary_heap_free(&heap);
        return 1;
    }

    if (ckit_binary_heap_free(&heap) != CKIT_OK) {
        fprintf(stderr, "ckit_binary_heap_free should return CKIT_OK\n");
        return 1;
    }

    return 0;
}
