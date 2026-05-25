#ifndef CKIT_MEMORY_ALLOCATOR_H
#define CKIT_MEMORY_ALLOCATOR_H

#include <stddef.h>

typedef void *(*ckit_alloc_fn)(void *ctx, size_t size);
typedef void *(*ckit_realloc_fn)(void *ctx, void *ptr, size_t size);
typedef void (*ckit_dealloc_fn)(void *ctx, void *ptr);

typedef enum ckit_allocator_features {
    CKIT_ALLOCATOR_FEATURE_DEALLOC = 1 << 0,
    CKIT_ALLOCATOR_FEATURE_REALLOC = 1 << 1,
    CKIT_ALLOCATOR_FEATURE_RESET = 1 << 2,
} ckit_allocator_features;

/* Generic allocator callback set used by containers. */
typedef struct ckit_allocator {
    /* User-supplied allocator state passed to all callbacks. */
    void *ctx;
    /* Bitmask of CKIT_ALLOCATOR_FEATURE_* values supported by this allocator. */
    unsigned int features;
    /* Allocate size bytes and return pointer or NULL on failure. */
    ckit_alloc_fn alloc;
    /* Resize ptr to size bytes and return new pointer or NULL on failure. */
    ckit_realloc_fn realloc;
    /* Release a pointer previously returned by alloc/realloc. */
    ckit_dealloc_fn dealloc;
} ckit_allocator;

/* Allocate size bytes through allocator when provided, otherwise malloc. */
void *ckit_malloc(ckit_allocator *allocator, size_t size);

/* Resize ptr through allocator when provided, otherwise realloc. */
void *ckit_realloc(ckit_allocator *allocator, void *ptr, size_t size);

/* Release ptr through allocator when provided, otherwise free. */
void ckit_dealloc(ckit_allocator *allocator, void *ptr);

#endif
