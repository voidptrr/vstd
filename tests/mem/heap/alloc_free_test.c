#include <stdio.h>

#include "ckit/mem/allocators/general_heap.h"

int main(void) {
    ckit_heap heap;
    size_t before;
    size_t after;
    int *a;
    int *b;

    ckit_heap_init(&heap, 2048U);
    before = ckit_heap_available(&heap);

    a = (int *)ckit_heap_alloc(&heap, sizeof(int));
    b = (int *)ckit_heap_alloc(&heap, sizeof(int));
    if (a == NULL || b == NULL) {
        fprintf(stderr, "heap alloc should return pointers\n");
        ckit_heap_free(&heap);
        return 1;
    }

    after = ckit_heap_available(&heap);
    if (after >= before) {
        fprintf(stderr, "available bytes should decrease after alloc\n");
        ckit_heap_free(&heap);
        return 1;
    }

    ckit_heap_dealloc(&heap, a);
    ckit_heap_dealloc(&heap, b);
    ckit_heap_free(&heap);
    return 0;
}
