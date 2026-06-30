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

#ifndef K4C_BINARY_HEAP_H
#define K4C_BINARY_HEAP_H

#include <stdbool.h>
#include <stddef.h>

#include "k4c/allocators/allocator.h"
#include "k4c/datastruct/iterator.h"
#include "k4c/error.h"

#define k4c_binary_heap_for_each(type, item, k4c_heap) \
    for (k4c_iterator item##_iter__ = k4c_binary_heap_get_iterator((k4c_heap)); \
         item##_iter__.next != NULL; \
         item##_iter__.next = NULL) \
    k4c_iterator_for_each(type, item, &item##_iter__)

/*
 * Opaque binary k4c_heap.
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
 * Backing storage is k4c_vector-like:
 *
 *   +-----+-----+-----+-----+-----+-----+-----+
 *   | [0] | [1] | [2] | [3] | [4] | [5] | [6] |
 *   +-----+-----+-----+-----+-----+-----+-----+
 *
 * For index i: left child = 2*i + 1, right child = 2*i + 2.
 */

/* Comparator callback: negative if a < b, zero if equal, positive if a > b. */
typedef int (*k4c_binary_heap_cmp_fn)(const void *lhs, const void *rhs);

typedef struct k4c_binary_heap k4c_binary_heap;

/* Create a k4c_heap for elements of size elem_size. NULL cmp uses byte-wise ordering. */
k4c_status k4c_binary_heap_create(
    size_t elem_size,
    k4c_binary_heap_cmp_fn cmp,
    k4c_allocator *k4c_allocator,
    k4c_binary_heap **out
);

/* Insert one element by copying elem_size bytes from element. */
k4c_status k4c_binary_heap_push(k4c_binary_heap *k4c_heap, const void *element);

/* Remove and return the top element pointer, or NULL when empty. */
void *k4c_binary_heap_pop(k4c_binary_heap *k4c_heap);

/* Return the top element pointer without removing it, or NULL when empty. */
const void *k4c_binary_heap_peek(const k4c_binary_heap *k4c_heap);

/* Return the number of stored elements. */
size_t k4c_binary_heap_size(const k4c_binary_heap *k4c_heap);

/* Return an k4c_iterator over k4c_heap backing storage order. */
k4c_iterator k4c_binary_heap_get_iterator(const k4c_binary_heap *k4c_heap);

/* Destroy and release owned storage. */
void k4c_binary_heap_destroy(k4c_binary_heap *k4c_heap);

#endif
