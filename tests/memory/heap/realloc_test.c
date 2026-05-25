#include <stdio.h>
#include <string.h>

#include "ckit/memory/allocators/general_heap.h"

int main(void) {
    ckit_heap *heap = ckit_heap_init(4096);
    char *ptr;

    ptr = (char *)ckit_heap_realloc(heap, NULL, 32);
    if (ptr == NULL) {
        fprintf(stderr, "realloc with NULL should allocate\n");
        ckit_heap_deinit(heap);
        return 1;
    }

    memcpy(ptr, "hello", 6);
    ptr = (char *)ckit_heap_realloc(heap, ptr, 128);
    if (ptr == NULL || memcmp(ptr, "hello", 6) != 0) {
        fprintf(stderr, "realloc should preserve existing bytes\n");
        ckit_heap_deinit(heap);
        return 1;
    }

    if (ckit_heap_realloc(heap, ptr, 0) != NULL) {
        fprintf(stderr, "realloc size 0 should return NULL\n");
        ckit_heap_deinit(heap);
        return 1;
    }

    ckit_heap_deinit(heap);
    return 0;
}
