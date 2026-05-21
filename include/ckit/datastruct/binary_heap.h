#ifndef CKIT_DATASTRUCT_BINARY_HEAP_H
#define CKIT_DATASTRUCT_BINARY_HEAP_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/compare.h"
#include "ckit/memory/allocators/allocator.h"

/*
 * Opaque binary heap.
 *
 * Logical tree view:
 *
 *                  top
 *                   |
 *                   v
 *                  [0]
 *                /     \
 *              [1]     [2]
 *             /   \   /   \
 *           [3]  [4] [5]  [6]
 *
 * Backing storage is vector-like:
 *
 *   +-----+-----+-----+-----+-----+-----+-----+
 *   | [0] | [1] | [2] | [3] | [4] | [5] | [6] |
 *   +-----+-----+-----+-----+-----+-----+-----+
 *
 * For index i: left child = 2*i + 1, right child = 2*i + 2.
 */

/* Comparator callback: negative if a < b, zero if equal, positive if a > b. */
typedef ckit_cmp_fn ckit_heap_cmp_fn;

typedef struct ckit_binary_heap ckit_binary_heap;

/* Create a heap for elements of size elem_size using cmp ordering. */
ckit_binary_heap *ckit_binary_heap_init(size_t elem_size, ckit_heap_cmp_fn cmp,
                                        ckit_allocator *allocator);

/* Insert one element by copying elem_size bytes from element. */
void ckit_binary_heap_push(ckit_binary_heap *heap, const void *element);

/* Remove and return the top element pointer, or NULL when empty. */
void *ckit_binary_heap_pop(ckit_binary_heap *heap);

/* Return the top element pointer without removing it, or NULL when empty. */
const void *ckit_binary_heap_peek(const ckit_binary_heap *heap);

/* Release owned storage and the heap handle. */
void ckit_binary_heap_free(ckit_binary_heap *heap);

/* Return the number of stored elements. */
size_t ckit_binary_heap_size(const ckit_binary_heap *heap);

/* Return whether the heap has zero elements. */
bool ckit_binary_heap_is_empty(const ckit_binary_heap *heap);

#endif
