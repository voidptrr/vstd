#ifndef CK_MEMORY_ALLOCATOR_H
#define CK_MEMORY_ALLOCATOR_H

#include <stddef.h>

typedef void *(*ck_alloc_fn)(void *ctx, size_t size);
typedef void *(*ck_realloc_fn)(void *ctx, void *ptr, size_t size);
typedef void (*ck_dealloc_fn)(void *ctx, void *ptr);

typedef enum ck_allocator_features {
    CK_ALLOCATOR_FEATURE_DEALLOC = 1 << 0,
    CK_ALLOCATOR_FEATURE_REALLOC = 1 << 1,
    CK_ALLOCATOR_FEATURE_RESET = 1 << 2,
} ck_allocator_features;

/* Generic allocator callback set used by containers. */
typedef struct ck_allocator {
    /* User-supplied allocator state passed to all callbacks. */
    void *ctx;
    /* Bitmask of CK_ALLOCATOR_FEATURE_* values supported by this allocator. */
    unsigned int features;
    /* Allocate size bytes and return pointer or NULL on failure. */
    ck_alloc_fn alloc;
    /* Resize ptr to size bytes and return new pointer or NULL on failure. */
    ck_realloc_fn realloc;
    /* Release a pointer previously returned by alloc/realloc. */
    ck_dealloc_fn dealloc;
} ck_allocator;

/* Allocate size bytes through allocator->alloc when provided, otherwise malloc. */
void *ck_malloc(ck_allocator *allocator, size_t size);

/* Resize ptr through allocator->realloc when provided, otherwise realloc. */
void *ck_realloc(ck_allocator *allocator, void *ptr, size_t size);

/* Release ptr through allocator->dealloc when advertised, otherwise free for NULL allocator. */
void ck_dealloc(ck_allocator *allocator, void *ptr);

#endif
