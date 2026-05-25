#include <stdio.h>

#include "ckit/datastruct/binary_heap.h"

static int cmp_int_asc(const void *a, const void *b) {
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    return (lhs > rhs) - (lhs < rhs);
}

int main(void) {
    int status = 0;
    ck_binary_heap *heap;

    heap = ck_binary_heap_init(sizeof(int), cmp_int_asc, NULL);

    if (ck_binary_heap_size(heap) != 0) {
        fprintf(stderr, "newly initialized heap should be empty\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_binary_heap_deinit(heap);
    return status;
}
