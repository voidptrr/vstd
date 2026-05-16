#ifndef CSTD_DATASTRUCT_BINARY_HEAP_H
#define CSTD_DATASTRUCT_BINARY_HEAP_H

#include <stdbool.h>
#include <stddef.h>

#include "cstd/datastruct/vector.h"
#include "cstd/status.h"

typedef int (*cstd_heap_cmp_fn)(const void *a, const void *b);

/*
 * Binary heap backed by cstd_vector.
 * +----------------------+----------------------+
 * | root (cstd_vector)   | cmp (cstd_heap_cmp_fn) |
 * +----------------------+----------------------+
 */
typedef struct cstd_binary_heap {
    cstd_vector root;
    cstd_heap_cmp_fn cmp;
} cstd_binary_heap;

/* Initialize a heap for elements of size elem_size using cmp ordering. */
cstd_status cstd_binary_heap_init(cstd_binary_heap *heap, size_t elem_size, cstd_heap_cmp_fn cmp);

/* Insert one element by copying elem_size bytes from element. */
cstd_status cstd_binary_heap_push(cstd_binary_heap *heap, const void *element);

/* Remove the top element and copy it into out. */
cstd_status cstd_binary_heap_pop(cstd_binary_heap *heap, void *out);

/* Copy the top element into out without removing it. */
cstd_status cstd_binary_heap_peek(const cstd_binary_heap *heap, void *out);

/* Release owned storage and reset heap to an empty state. */
cstd_status cstd_binary_heap_free(cstd_binary_heap *heap);

/* Return the number of stored elements. */
size_t cstd_binary_heap_size(const cstd_binary_heap *heap);

/* Return whether the heap has zero elements. */
bool cstd_binary_heap_is_empty(const cstd_binary_heap *heap);

#endif
