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

#ifndef K4C_VECTOR_H
#define K4C_VECTOR_H

#include <stdbool.h>
#include <stddef.h>

#include "k4c/ds/iterator.h"
#include "k4c/error.h"
#include "k4c/memory/allocator.h"

#define k4c_vector_for_each(type, item, k4c_vector) \
    for (k4c_iterator item##_iter__ = k4c_vector_get_iterator((k4c_vector)); \
         item##_iter__.next != NULL; \
         item##_iter__.next = NULL) \
    k4c_iterator_for_each(type, item, &item##_iter__)

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
typedef struct k4c_vector k4c_vector;

/* Comparator callback: negative if lhs < rhs, zero if equal, positive if lhs > rhs. */
typedef int (*k4c_vector_cmp_fn)(const void *lhs, const void *rhs);

/* Map callback used when collecting iterator items into a vector. */
typedef void (*k4c_vector_collect_map_fn)(void *context, const void *src, void *dst);

/* Create a k4c_vector with element size elem_size. */
k4c_status k4c_vector_create(size_t elem_size, k4c_allocator *k4c_allocator, k4c_vector **out);

/* Create a k4c_vector with element size elem_size and at least capacity slots. */
k4c_status k4c_vector_create_with_capacity(
    size_t elem_size,
    size_t capacity,
    k4c_allocator *k4c_allocator,
    k4c_vector **out
);

/* Ensure k4c_vector can hold at least capacity elements without growing. */
k4c_status k4c_vector_reserve(k4c_vector *k4c_vector, size_t capacity);

/* Append one element by copying elem_size bytes from element. */
k4c_status k4c_vector_push(k4c_vector *k4c_vector, const void *element);

/* Remove and return the last element pointer, or NULL when empty. */
void *k4c_vector_pop(k4c_vector *k4c_vector);

/* Return item at index, or NULL when out of range. */
void *k4c_vector_get(const k4c_vector *k4c_vector, size_t index);

/* Return the configured element size. */
size_t k4c_vector_elem_size(const k4c_vector *k4c_vector);

/* Return the number of elements k4c_vector can store without growing. */
size_t k4c_vector_capacity(const k4c_vector *k4c_vector);

/* Return the backing storage pointer. */
void *k4c_vector_data(const k4c_vector *k4c_vector);

/* Remove item at index by moving the last item into its slot. */
void *k4c_vector_swap_remove(k4c_vector *k4c_vector, size_t index);

/* Return the number of stored elements. */
size_t k4c_vector_size(const k4c_vector *k4c_vector);

/* Return an k4c_iterator over k4c_vector from index 0 to size - 1. */
k4c_iterator k4c_vector_get_iterator(const k4c_vector *k4c_vector);

/* Collect remaining source items by copying elem_size bytes into a new k4c_vector. */
k4c_status k4c_vector_collect(
    k4c_iterator *source,
    size_t elem_size,
    k4c_allocator *k4c_allocator,
    k4c_vector **out
);

/* Map each remaining source item into dst_elem_size bytes and collect into a new k4c_vector. */
k4c_status k4c_vector_collect_map(
    k4c_iterator *source,
    size_t dst_elem_size,
    k4c_vector_collect_map_fn map,
    void *context,
    k4c_allocator *k4c_allocator,
    k4c_vector **out
);

/* Return the first sorted index whose item is not less than key. */
size_t k4c_vector_lower_bound(const k4c_vector *k4c_vector, const void *key, k4c_vector_cmp_fn cmp);

/* Return the first sorted index whose item is greater than key. */
size_t k4c_vector_upper_bound(const k4c_vector *k4c_vector, const void *key, k4c_vector_cmp_fn cmp);

/* Return the sorted index containing key, or k4c_vector_size(k4c_vector) when absent. */
size_t k4c_vector_binary_search(
    const k4c_vector *k4c_vector,
    const void *key,
    k4c_vector_cmp_fn cmp
);

/* Destroy and release owned storage. */
void k4c_vector_destroy(k4c_vector *k4c_vector);

#endif
