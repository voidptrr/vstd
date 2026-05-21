#include <stdio.h>

#include "ckit/datastruct/binary_heap.h"

static int cmp_int_asc(const void *a, const void *b) {
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    return (lhs > rhs) - (lhs < rhs);
}

int main(void) {
    ckit_binary_heap heap;
    int value = 3;
    const int *out;

    ckit_binary_heap_init(&heap, sizeof(int), cmp_int_asc, NULL);

    if (ckit_binary_heap_peek(&heap) != NULL) {
        fprintf(stderr, "peek on empty heap should return NULL\n");
        ckit_binary_heap_free(&heap);
        return 1;
    }

    ckit_binary_heap_push(&heap, &value);
    out = (const int *)ckit_binary_heap_peek(&heap);
    if (out == NULL || *out != 3) {
        fprintf(stderr, "peek should return current top\n");
        ckit_binary_heap_free(&heap);
        return 1;
    }

    ckit_binary_heap_free(&heap);
    return 0;
}
