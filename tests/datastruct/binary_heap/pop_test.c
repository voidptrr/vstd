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
    int values[] = {4, 1, 3};

    heap = ck_binary_heap_init(sizeof(int), cmp_int_asc, NULL);
    for (size_t i = 0; i < 3; i++) {
        ck_binary_heap_push(heap, &values[i]);
    }

    for (size_t i = 0; i < 3; i++) {
        int *out = (int *)ck_binary_heap_pop(heap);
        if (out == NULL) {
            fprintf(stderr, "pop should return non-NULL while heap has items\n");
            status = 1;
            goto cleanup;
        }
    }

    if (ck_binary_heap_pop(heap) != NULL) {
        fprintf(stderr, "pop on empty heap should return NULL\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_binary_heap_deinit(heap);
    return status;
}
