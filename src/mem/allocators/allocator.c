#include <stdio.h>
#include <stdlib.h>

#include "ckit/mem/allocators/allocator.h"

void *ckit_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fputs("fatal: ckit_malloc out of memory\n", stderr);
        abort();
    }
    return ptr;
}

void *ckit_realloc(void *ptr, size_t size) {
    if (size == 0U) {
        free(ptr);
        return NULL;
    }

    void *new_ptr = realloc(ptr, size);
    if (new_ptr == NULL) {
        fputs("fatal: ckit_realloc out of memory\n", stderr);
        abort();
    }
    return new_ptr;
}
