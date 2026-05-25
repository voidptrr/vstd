#ifndef CKIT_MEMORY_GENERAL_HEAP_H
#define CKIT_MEMORY_GENERAL_HEAP_H

#include <stddef.h>

#include "ckit/memory/allocators/allocator.h"

/*
 * Free-list heap memory model:
 *
 * +----------------------+----------------------+----------------------+----------------------+
 * | block header (A)     | payload (A)          | block header (B)     | payload (B)          |
 * +----------------------+----------------------+----------------------+----------------------+
 *
 * Each block header tracks:
 * - payload size
 * - free/used state
 * - intrusive doubly linked list node
 *
 * Linked-list view of blocks:
 *
 *   blocks -> [A free] <-> [B used] <-> [C free] -> NULL
 *
 * Allocation strategy:
 * - first-fit scan from head
 * - split large free blocks
 * - coalesce neighboring free blocks on dealloc
 */
typedef struct ckit_heap ckit_heap;

/*
 * Initialize a heap with `capacity` bytes of internal storage.
 */
ckit_heap *ckit_heap_init(size_t capacity);

/*
 * Return an allocator adapter that routes through this heap.
 */
ckit_allocator ckit_heap_allocator(ckit_heap *heap);

/*
 * Allocate `size` bytes from heap.
 * Returns NULL when no suitable free block exists.
 */
void *ckit_heap_alloc(ckit_heap *heap, size_t size);

/*
 * Free a pointer previously returned by ckit_heap_alloc/ckit_heap_realloc.
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

/*
 * Deinitialize and release heap resources.
 */
void ckit_heap_deinit(ckit_heap *heap);

#endif
