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
    int value = 3;
    const int *out;

    heap = ck_binary_heap_init(sizeof(int), cmp_int_asc, NULL);

    if (ck_binary_heap_peek(heap) != NULL) {
        fprintf(stderr, "peek on empty heap should return NULL\n");
        status = 1;
        goto cleanup;
    }

    ck_binary_heap_push(heap, &value);
    out = (const int *)ck_binary_heap_peek(heap);
    if (out == NULL || *out != 3) {
        fprintf(stderr, "peek should return current top\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_binary_heap_deinit(heap);
    return status;
}
