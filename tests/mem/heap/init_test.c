#include <stdio.h>

#include "ckit/mem/allocators/general_heap.h"

int main(void) {
    ckit_heap heap;

    ckit_heap_init(&heap, 1024U);
    if (ckit_heap_capacity(&heap) == 0U || ckit_heap_available(&heap) == 0U) {
        fprintf(stderr, "heap should initialize with capacity and free space\n");
        ckit_heap_free(&heap);
        return 1;
    }

    ckit_heap_free(&heap);
    return 0;
}
