#ifndef CKIT_DATASTRUCT_BINARY_HEAP_H
#define CKIT_DATASTRUCT_BINARY_HEAP_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/datastruct/vector.h"

/*
 * Binary heap backed by ckit_vector.
 * +----------------------+------------------------+
 * | root (ckit_vector)   | cmp (ckit_heap_cmp_fn) |
 * +----------------------+------------------------+
 */

/* Comparator callback: negative if a < b, zero if equal, positive if a > b. */
typedef int (*ckit_heap_cmp_fn)(const void *a, const void *b);

/* Heap object wrapping vector storage and comparator policy. */
typedef struct ckit_binary_heap {
    ckit_vector root;
    ckit_heap_cmp_fn cmp;
} ckit_binary_heap;

/* Initialize a heap for elements of size elem_size using cmp ordering. */
void ckit_binary_heap_init(ckit_binary_heap *heap, size_t elem_size, ckit_heap_cmp_fn cmp,
                           ckit_allocator *allocator);

/* Insert one element by copying elem_size bytes from element. */
void ckit_binary_heap_push(ckit_binary_heap *heap, const void *element);

/* Remove and return the top element pointer, or NULL when empty. */
void *ckit_binary_heap_pop(ckit_binary_heap *heap);

/* Return the top element pointer without removing it, or NULL when empty. */
const void *ckit_binary_heap_peek(const ckit_binary_heap *heap);

/* Release owned storage and reset heap to an empty state. */
void ckit_binary_heap_free(ckit_binary_heap *heap);

/* Return the number of stored elements. */
size_t ckit_binary_heap_size(const ckit_binary_heap *heap);

/* Return whether the heap has zero elements. */
bool ckit_binary_heap_is_empty(const ckit_binary_heap *heap);

#endif
