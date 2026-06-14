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

/* Create a vector with element size elem_size. */
ck_vector *ck_vector_create(size_t elem_size, ck_allocator *allocator);

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

/* Destroy and release owned storage. */
void ck_vector_destroy(ck_vector *vector);

#endif
