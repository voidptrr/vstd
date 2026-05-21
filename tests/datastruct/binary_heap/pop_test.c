#include <stdio.h>

#include "ckit/datastruct/binary_heap.h"

static int cmp_int_asc(const void *a, const void *b) {
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    return (lhs > rhs) - (lhs < rhs);
}

int main(void) {
    ckit_binary_heap heap;
    int values[] = {4, 1, 3};

    ckit_binary_heap_init(&heap, sizeof(int), cmp_int_asc, NULL);
    for (size_t i = 0; i < 3; i++) {
        ckit_binary_heap_push(&heap, &values[i]);
    }

    for (size_t i = 0; i < 3; i++) {
        int *out = (int *)ckit_binary_heap_pop(&heap);
        if (out == NULL) {
            fprintf(stderr, "pop should return non-NULL while heap has items\n");
            ckit_binary_heap_free(&heap);
            return 1;
        }
    }

    if (ckit_binary_heap_pop(&heap) != NULL) {
        fprintf(stderr, "pop on empty heap should return NULL\n");
        ckit_binary_heap_free(&heap);
        return 1;
    }

    ckit_binary_heap_free(&heap);
    return 0;
}
