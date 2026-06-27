/*
 * MIT License
 *
 * Copyright (c) 2026 Tommaso Bruno
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stddef.h>

#include "vstd/ds/iterator.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"

#define vector_for_each(type, item, vector) \
    for (iterator item##_iter__ = vector_get_iterator((vector)); item##_iter__.next != NULL; \
         item##_iter__.next = NULL) \
    iterator_for_each(type, item, &item##_iter__)

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
typedef struct vector vector;

/* Comparator callback: negative if lhs < rhs, zero if equal, positive if lhs > rhs. */
typedef int (*vector_cmp_fn)(const void *lhs, const void *rhs);

/* Create a vector with element size elem_size. */
NODISCARD status vector_create(size_t elem_size, allocator *allocator, vector **out);

/* Create a vector with element size elem_size and at least capacity slots. */
NODISCARD status
vector_create_with_capacity(size_t elem_size, size_t capacity, allocator *allocator, vector **out);

/* Ensure vector can hold at least capacity elements without growing. */
NODISCARD status vector_reserve(vector *vector, size_t capacity);

/* Append one element by copying elem_size bytes from element. */
NODISCARD status vector_push(vector *vector, const void *element);

/* Remove and return the last element pointer, or NULL when empty. */
void *vector_pop(vector *vector);

/* Return item at index, or NULL when out of range. */
void *vector_get(vector *vector, size_t index);

/* Return const item at index, or NULL when out of range. */
const void *vector_get_const(const vector *vector, size_t index);

/* Return the configured element size. */
size_t vector_elem_size(const vector *vector);

/* Return the number of elements vector can store without growing. */
size_t vector_capacity(const vector *vector);

/* Return the backing storage pointer. */
void *vector_data(vector *vector);

/* Return the const backing storage pointer. */
const void *vector_data_const(const vector *vector);

/* Remove item at index by moving the last item into its slot. */
void *vector_swap_remove(vector *vector, size_t index);

/* Return the number of stored elements. */
size_t vector_size(const vector *vector);

/* Return an iterator over vector from index 0 to size - 1. */
iterator vector_get_iterator(const vector *vector);

/* Return the first sorted index whose item is not less than key. */
size_t vector_lower_bound(const vector *vector, const void *key, vector_cmp_fn cmp);

/* Return the first sorted index whose item is greater than key. */
size_t vector_upper_bound(const vector *vector, const void *key, vector_cmp_fn cmp);

/* Return the sorted index containing key, or vector_size(vector) when absent. */
size_t vector_binary_search(const vector *vector, const void *key, vector_cmp_fn cmp);

/* Destroy and release owned storage. */
void vector_destroy(vector *vector);

#endif
