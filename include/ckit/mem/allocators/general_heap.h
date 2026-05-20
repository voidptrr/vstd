#ifndef CKIT_MEM_GENERAL_HEAP_H
#define CKIT_MEM_GENERAL_HEAP_H

#include <stddef.h>

#include "ckit/mem/allocators/allocator.h"

/*
 * Free-list heap memory model (inside heap->buffer):
 *
 * +----------------------+----------------------+----------------------+----------------------+
 * | block header (A)     | payload (A)          | block header (B)     | payload (B)          |
 * +----------------------+----------------------+----------------------+----------------------+
 *
 * Each block header tracks:
 * - payload size
 * - free/used state
 * - previous/next block links
 *
 * Linked-list view of blocks:
 *
 *   heap->head -> [A free] <-> [B used] <-> [C free] -> NULL
 *
 * Allocation strategy:
 * - first-fit scan from head
 * - split large free blocks
 * - coalesce neighboring free blocks on dealloc
 */
typedef struct ckit_heap {
    /* Base address of the heap-owned contiguous memory region. */
    void *buffer;
    /* Total bytes managed by this heap region. */
    size_t capacity;
    /* Internal pointer to first block header in the region. */
    void *head;
} ckit_heap;

/*
 * Initialize a heap with `capacity` bytes of internal storage.
 * Returns an allocator adapter that routes through this heap.
 */
ckit_allocator ckit_heap_init(ckit_heap *heap, size_t capacity);

/*
 * Release heap resources and reset heap state.
 */
void ckit_heap_free(ckit_heap *heap);

/*
 * Allocate `size` bytes from heap.
 * Returns NULL when no suitable free block exists.
 */
void *ckit_heap_alloc(ckit_heap *heap, size_t size);

/*
 * Free a pointer previously returned by ckit_heap_alloc/ckit_heap_realloc.
 * NULL is ignored.
 */
void ckit_heap_dealloc(ckit_heap *heap, void *ptr);

/*
 * Resize an existing heap allocation.
 * Behaves like realloc: NULL ptr allocates, size 0 frees and returns NULL.
 */
void *ckit_heap_realloc(ckit_heap *heap, void *ptr, size_t size);

/* Total managed bytes (including internal metadata overhead). */
size_t ckit_heap_capacity(const ckit_heap *heap);

/* Sum of currently free payload bytes. */
size_t ckit_heap_available(const ckit_heap *heap);

#endif
