#include <stdio.h>

#include "ckit/memory/allocators/general_heap.h"

int main(void) {
    int status = 0;
    ck_heap *heap = ck_heap_init(1024);

    if (ck_heap_capacity(heap) == 0 || ck_heap_available(heap) == 0) {
        fprintf(stderr, "heap should init with capacity and free space\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_heap_deinit(heap);
    return status;
}
