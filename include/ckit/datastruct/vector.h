#ifndef CKIT_DATASTRUCT_VECTOR_H
#define CKIT_DATASTRUCT_VECTOR_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/memory/allocators/allocator.h"

/*
 * Opaque generic contiguous dynamic array.
 *
 * Logical view:
 *
 *   index:   0      1      2             size - 1
 *          +------+------+------+ ... +----------+
 *   items: |  T   |  T   |  T   |     |    T     |
 *          +------+------+------+ ... +----------+
 *
 * - push appends at the right edge
 * - pop removes from the right edge
 * - get returns an item by index in O(1)
 */
typedef struct ckit_vector ckit_vector;

/* Create a vector with element size elem_size. */
ckit_vector *ckit_vector_init(size_t elem_size, ckit_allocator *allocator);

/* Append one element by copying elem_size bytes from element. */
void ckit_vector_push(ckit_vector *vector, const void *element);

/* Remove and return the last element pointer, or NULL when empty. */
void *ckit_vector_pop(ckit_vector *vector);

/* Return item at index, or NULL when out of range. */
void *ckit_vector_get(ckit_vector *vector, size_t index);

/* Return const item at index, or NULL when out of range. */
const void *ckit_vector_get_const(const ckit_vector *vector, size_t index);

/* Return the configured element size. */
size_t ckit_vector_elem_size(const ckit_vector *vector);

/* Remove item at index by moving the last item into its slot. */
void *ckit_vector_swap_remove(ckit_vector *vector, size_t index);

/* Release owned storage and the vector handle. */
void ckit_vector_free(ckit_vector *vector);

/* Return the number of stored elements. */
size_t ckit_vector_size(const ckit_vector *vector);

/* Return whether the vector has zero elements. */
bool ckit_vector_is_empty(const ckit_vector *vector);

#endif
