#ifndef CK_DATASTRUCT_BINARY_HEAP_H
#define CK_DATASTRUCT_BINARY_HEAP_H

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
typedef ck_cmp_fn ck_heap_cmp_fn;

typedef struct ck_binary_heap ck_binary_heap;

/* Initialize a heap for elements of size elem_size using cmp ordering. */
ck_binary_heap *ck_binary_heap_init(size_t elem_size, ck_heap_cmp_fn cmp, ck_allocator *allocator);

/* Insert one element by copying elem_size bytes from element. */
void ck_binary_heap_push(ck_binary_heap *heap, const void *element);

/* Remove and return the top element pointer, or NULL when empty. */
void *ck_binary_heap_pop(ck_binary_heap *heap);

/* Return the top element pointer without removing it, or NULL when empty. */
const void *ck_binary_heap_peek(const ck_binary_heap *heap);

/* Return the number of stored elements. */
size_t ck_binary_heap_size(const ck_binary_heap *heap);

/* Deinitialize and release owned storage. */
void ck_binary_heap_deinit(ck_binary_heap *heap);

#endif
