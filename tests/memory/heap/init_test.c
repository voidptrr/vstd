#include <stdio.h>

#include "ckit/memory/allocators/general_heap.h"

int main(void) {
    ckit_heap *heap = ckit_heap_init(1024);

    if (ckit_heap_capacity(heap) == 0 || ckit_heap_available(heap) == 0) {
        fprintf(stderr, "heap should init with capacity and free space\n");
        ckit_heap_deinit(heap);
        return 1;
    }

    ckit_heap_deinit(heap);
    return 0;
}
