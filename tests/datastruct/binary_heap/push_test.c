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
    int values[] = {5, 2, 8, 1};
    const int *top;

    heap = ck_binary_heap_init(sizeof(int), cmp_int_asc, NULL);
    for (size_t i = 0; i < 4; i++) {
        ck_binary_heap_push(heap, &values[i]);
    }

    top = (const int *)ck_binary_heap_peek(heap);
    if (top == NULL || *top != 1 || ck_binary_heap_size(heap) != 4) {
        fprintf(stderr, "push should maintain heap invariant\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_binary_heap_deinit(heap);
    return status;
}
