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

#ifndef VSTD_VECTOR_H
#define VSTD_VECTOR_H

#include <stdbool.h>
#include <stddef.h>

#include "vstd/datastruct/iterator.h"
#include "vstd/memory/allocator.h"

#define VS_VECTOR_PUSH_AS(vector, type, value) \
    do { \
        type vs_vector_push_value__ = (value); \
        vs_vector_push((vector), &vs_vector_push_value__); \
    } while (0)

#define VS_VECTOR_FOR_EACH(type, item, vector) \
    for (vs_iterator item##_vs_iter__ = vs_vector_get_iterator((vector)); \
         item##_vs_iter__.next != NULL; \
         item##_vs_iter__.next = NULL) \
    VS_ITERATOR_FOR_EACH(type, item, &item##_vs_iter__)

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
typedef struct vs_vector vs_vector;

/* Comparator callback: negative if lhs < rhs, zero if equal, positive if lhs > rhs. */
typedef int (*vs_vector_cmp_fn)(const void *lhs, const void *rhs);

/* Create a vector with element size elem_size. */
vs_vector *vs_vector_create(size_t elem_size, vs_allocator *allocator);

/* Create a vector with element size elem_size and at least capacity slots. */
vs_vector *vs_vector_create_with_capacity(
    size_t elem_size,
    size_t capacity,
    vs_allocator *allocator
);

/* Ensure vector can hold at least capacity elements without growing. */
void vs_vector_reserve(vs_vector *vector, size_t capacity);

/* Append one element by copying elem_size bytes from element. */
void vs_vector_push(vs_vector *vector, const void *element);

/* Remove and return the last element pointer, or NULL when empty. */
void *vs_vector_pop(vs_vector *vector);

/* Return item at index, or NULL when out of range. */
void *vs_vector_get(vs_vector *vector, size_t index);

/* Return const item at index, or NULL when out of range. */
const void *vs_vector_get_const(const vs_vector *vector, size_t index);

/* Return the configured element size. */
size_t vs_vector_elem_size(const vs_vector *vector);

/* Return the number of elements vector can store without growing. */
size_t vs_vector_capacity(const vs_vector *vector);

/* Return the backing storage pointer. */
void *vs_vector_data(vs_vector *vector);

/* Return the const backing storage pointer. */
const void *vs_vector_data_const(const vs_vector *vector);

/* Remove item at index by moving the last item into its slot. */
void *vs_vector_swap_remove(vs_vector *vector, size_t index);

/* Return the number of stored elements. */
size_t vs_vector_size(const vs_vector *vector);

/* Return an iterator over vector from index 0 to size - 1. */
vs_iterator vs_vector_get_iterator(const vs_vector *vector);

/* Return the first sorted index whose item is not less than key. */
size_t vs_vector_lower_bound(const vs_vector *vector, const void *key, vs_vector_cmp_fn cmp);

/* Return the first sorted index whose item is greater than key. */
size_t vs_vector_upper_bound(const vs_vector *vector, const void *key, vs_vector_cmp_fn cmp);

/* Return the sorted index containing key, or vs_vector_size(vector) when absent. */
size_t vs_vector_binary_search(const vs_vector *vector, const void *key, vs_vector_cmp_fn cmp);

/* Destroy and release owned storage. */
void vs_vector_destroy(vs_vector *vector);

#endif
