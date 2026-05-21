#include <stdio.h>

#include "ckit/datastruct/binary_heap.h"

static int cmp_int_asc(const void *a, const void *b) {
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    return (lhs > rhs) - (lhs < rhs);
}

int main(void) {
    ckit_binary_heap heap;
    int values[] = {5, 2, 8, 1};
    const int *top;

    ckit_binary_heap_init(&heap, sizeof(int), cmp_int_asc, NULL);
    for (size_t i = 0; i < 4; i++) {
        ckit_binary_heap_push(&heap, &values[i]);
    }

    top = (const int *)ckit_binary_heap_peek(&heap);
    if (top == NULL || *top != 1 || ckit_binary_heap_size(&heap) != 4U) {
        fprintf(stderr, "push should maintain heap invariant\n");
        ckit_binary_heap_free(&heap);
        return 1;
    }

    ckit_binary_heap_free(&heap);
    return 0;
}
