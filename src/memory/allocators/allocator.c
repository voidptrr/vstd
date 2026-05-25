#include <stdlib.h>

#include "ckit/common/panic.h"
#include "ckit/memory/allocators/allocator.h"

void *ck_malloc(ck_allocator *allocator, size_t size) {
    void *ptr = NULL;
    if (allocator == NULL || allocator->alloc == NULL) {
        ptr = malloc(size);
    } else {
        ptr = allocator->alloc(allocator->ctx, size);
    }

    if (ptr == NULL) {
        ck_panic("fatal: ck_malloc out of memory");
    }
    return ptr;
}

void *ck_realloc(ck_allocator *allocator, void *ptr, size_t size) {
    if (size == 0) {
        ck_dealloc(allocator, ptr);
        return NULL;
    }

    void *new_ptr = NULL;
    if (allocator == NULL || allocator->realloc == NULL) {
        new_ptr = realloc(ptr, size);
    } else {
        new_ptr = allocator->realloc(allocator->ctx, ptr, size);
    }

    if (new_ptr == NULL) {
        ck_panic("fatal: ck_realloc out of memory");
    }
    return new_ptr;
}

void ck_dealloc(ck_allocator *allocator, void *ptr) {
    if (allocator == NULL) {
        free(ptr);
        return;
    }

    if ((allocator->features & CK_ALLOCATOR_FEATURE_DEALLOC) == 0) {
        return;
    }

    CK_ASSERT(allocator->dealloc != NULL, "fatal: ck_dealloc invalid arguments");
    allocator->dealloc(allocator->ctx, ptr);
}
