#include <stdio.h>
#include <string.h>

#include "ckit/mem/allocators/general_heap.h"

int main(void) {
    ckit_heap heap;
    char *ptr;

    ckit_heap_init(&heap, 4096U);

    ptr = (char *)ckit_heap_realloc(&heap, NULL, 32U);
    if (ptr == NULL) {
        fprintf(stderr, "realloc with NULL should allocate\n");
        ckit_heap_free(&heap);
        return 1;
    }

    memcpy(ptr, "hello", 6U);
    ptr = (char *)ckit_heap_realloc(&heap, ptr, 128U);
    if (ptr == NULL || memcmp(ptr, "hello", 6U) != 0) {
        fprintf(stderr, "realloc should preserve existing bytes\n");
        ckit_heap_free(&heap);
        return 1;
    }

    if (ckit_heap_realloc(&heap, ptr, 0U) != NULL) {
        fprintf(stderr, "realloc size 0 should return NULL\n");
        ckit_heap_free(&heap);
        return 1;
    }

    ckit_heap_free(&heap);
    return 0;
}
