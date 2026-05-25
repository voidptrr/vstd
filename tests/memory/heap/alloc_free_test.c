#include <stdio.h>

#include "ckit/memory/allocators/general_heap.h"

int main(void) {
    int status = 0;
    ck_heap *heap = ck_heap_init(2048);
    size_t before;
    size_t after;
    int *a;
    int *b;

    before = ck_heap_available(heap);

    a = (int *)ck_heap_alloc(heap, sizeof(int));
    b = (int *)ck_heap_alloc(heap, sizeof(int));
    if (a == NULL || b == NULL) {
        fprintf(stderr, "heap alloc should return pointers\n");
        status = 1;
        goto cleanup;
    }

    after = ck_heap_available(heap);
    if (after >= before) {
        fprintf(stderr, "available bytes should decrease after alloc\n");
        status = 1;
        goto cleanup;
    }

    ck_heap_dealloc(heap, a);
    ck_heap_dealloc(heap, b);
cleanup:
    ck_heap_deinit(heap);
    return status;
}
