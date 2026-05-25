#include <stdio.h>
#include <string.h>

#include "ckit/memory/allocators/general_heap.h"

int main(void) {
    int status = 0;
    ck_heap *heap = ck_heap_init(4096);
    char *ptr;

    ptr = (char *)ck_heap_realloc(heap, NULL, 32);
    if (ptr == NULL) {
        fprintf(stderr, "realloc with NULL should allocate\n");
        status = 1;
        goto cleanup;
    }

    memcpy(ptr, "hello", 6);
    ptr = (char *)ck_heap_realloc(heap, ptr, 128);
    if (ptr == NULL || memcmp(ptr, "hello", 6) != 0) {
        fprintf(stderr, "realloc should preserve existing bytes\n");
        status = 1;
        goto cleanup;
    }

    if (ck_heap_realloc(heap, ptr, 0) != NULL) {
        fprintf(stderr, "realloc size 0 should return NULL\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_heap_deinit(heap);
    return status;
}
