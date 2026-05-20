#ifndef CKIT_MEM_ALLOCATOR_H
#define CKIT_MEM_ALLOCATOR_H

#include <stddef.h>

typedef void *(*ckit_alloc_fn)(void *ctx, size_t size);
typedef void *(*ckit_realloc_fn)(void *ctx, void *ptr, size_t size);
typedef void (*ckit_dealloc_fn)(void *ctx, void *ptr);

/* Generic allocator callback set used by containers. */
typedef struct ckit_allocator {
    /* User-supplied allocator state passed to all callbacks. */
    void *ctx;
    /* Allocate size bytes and return pointer or NULL on failure. */
    ckit_alloc_fn alloc;
    /* Resize ptr to size bytes and return new pointer or NULL on failure. */
    ckit_realloc_fn realloc;
    /* Release a pointer previously returned by alloc/realloc. */
    ckit_dealloc_fn dealloc;
} ckit_allocator;

/*
 * Allocate size bytes.
 * Panics (prints fatal message and aborts) on out-of-memory.
 */
void *ckit_malloc(size_t size);

/*
 * Resize ptr to size bytes.
 * Panics (prints fatal message and aborts) on out-of-memory when size > 0.
 */
void *ckit_realloc(void *ptr, size_t size);

/* Print fatal message and abort process. */
void ckit_panic(const char *message);

#endif
