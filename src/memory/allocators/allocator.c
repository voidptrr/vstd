#include <stdlib.h>

#include "ckit/common/panic.h"
#include "ckit/memory/allocators/allocator.h"

void *ckit_malloc(ckit_allocator *allocator, size_t size) {
    void *ptr = NULL;
    if (allocator == NULL || allocator->alloc == NULL) {
        ptr = malloc(size);
    } else {
        ptr = allocator->alloc(allocator->ctx, size);
    }

    if (ptr == NULL) {
        ckit_panic("fatal: ckit_malloc out of memory");
    }
    return ptr;
}

void *ckit_realloc(ckit_allocator *allocator, void *ptr, size_t size) {
    if (size == 0U) {
        ckit_dealloc(allocator, ptr);
        return NULL;
    }

    void *new_ptr = NULL;
    if (allocator == NULL || allocator->realloc == NULL) {
        new_ptr = realloc(ptr, size);
    } else {
        new_ptr = allocator->realloc(allocator->ctx, ptr, size);
    }

    if (new_ptr == NULL) {
        ckit_panic("fatal: ckit_realloc out of memory");
    }
    return new_ptr;
}

void ckit_dealloc(ckit_allocator *allocator, void *ptr) {
    if (allocator == NULL) {
        free(ptr);
        return;
    }

    CKIT_ASSERT(allocator->dealloc != NULL, "fatal: ckit_dealloc invalid arguments");
    allocator->dealloc(allocator->ctx, ptr);
}
