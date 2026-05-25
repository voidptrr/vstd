#ifndef CK_MEMORY_GENERAL_HEAP_H
#define CK_MEMORY_GENERAL_HEAP_H

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
typedef struct ck_heap ck_heap;

/*
 * Initialize a heap with `capacity` bytes of internal storage.
 */
ck_heap *ck_heap_init(size_t capacity);

/*
 * Return an allocator adapter that routes through this heap.
 */
ck_allocator ck_heap_allocator(ck_heap *heap);

/*
 * Allocate `size` bytes from heap.
 * Returns NULL when no suitable free block exists.
 */
void *ck_heap_alloc(ck_heap *heap, size_t size);

/*
 * Free a pointer previously returned by ck_heap_alloc/ck_heap_realloc.
 */
void ck_heap_dealloc(ck_heap *heap, void *ptr);

/*
 * Resize an existing heap allocation.
 * Behaves like realloc: NULL ptr allocates, size 0 frees and returns NULL.
 */
void *ck_heap_realloc(ck_heap *heap, void *ptr, size_t size);

/* Total managed bytes (including internal metadata overhead). */
size_t ck_heap_capacity(const ck_heap *heap);

/* Sum of currently free payload bytes. */
size_t ck_heap_available(const ck_heap *heap);

/*
 * Deinitialize and release heap resources.
 */
void ck_heap_deinit(ck_heap *heap);

#endif
