#include <stdio.h>

#include "ckit/datastruct/binary_heap.h"

static int cmp_int_asc(const void *a, const void *b) {
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    return (lhs > rhs) - (lhs < rhs);
}

int main(void) {
    ckit_binary_heap *heap;

    heap = ckit_binary_heap_init(sizeof(int), cmp_int_asc, NULL);

    if (!ckit_binary_heap_is_empty(heap) || ckit_binary_heap_size(heap) != 0U) {
        fprintf(stderr, "newly initialized heap should be empty\n");
        ckit_binary_heap_free(heap);
        return 1;
    }

    ckit_binary_heap_free(heap);

    return 0;
}
