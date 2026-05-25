#ifndef CK_DATASTRUCT_VECTOR_H
#define CK_DATASTRUCT_VECTOR_H

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
typedef struct ck_vector ck_vector;

/* Initialize a vector with element size elem_size. */
ck_vector *ck_vector_init(size_t elem_size, ck_allocator *allocator);

/* Append one element by copying elem_size bytes from element. */
void ck_vector_push(ck_vector *vector, const void *element);

/* Remove and return the last element pointer, or NULL when empty. */
void *ck_vector_pop(ck_vector *vector);

/* Return item at index, or NULL when out of range. */
void *ck_vector_get(ck_vector *vector, size_t index);

/* Return const item at index, or NULL when out of range. */
const void *ck_vector_get_const(const ck_vector *vector, size_t index);

/* Return the configured element size. */
size_t ck_vector_elem_size(const ck_vector *vector);

/* Remove item at index by moving the last item into its slot. */
void *ck_vector_swap_remove(ck_vector *vector, size_t index);

/* Return the number of stored elements. */
size_t ck_vector_size(const ck_vector *vector);

/* Deinitialize and release owned storage. */
void ck_vector_deinit(ck_vector *vector);

#endif
