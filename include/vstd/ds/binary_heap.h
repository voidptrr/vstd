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

#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#include <stdbool.h>
#include <stddef.h>

#include "vstd/ds/iterator.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"

#define binary_heap_for_each(type, item, heap) \
    for (iterator item##_iter__ = binary_heap_get_iterator((heap)); item##_iter__.next != NULL; \
         item##_iter__.next = NULL) \
    iterator_for_each(type, item, &item##_iter__)

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
typedef int (*binary_heap_cmp_fn)(const void *lhs, const void *rhs);

typedef struct binary_heap binary_heap;

/* Create a heap for elements of size elem_size. NULL cmp uses byte-wise ordering. */
NODISCARD status binary_heap_create(
    size_t elem_size,
    binary_heap_cmp_fn cmp,
    allocator *allocator,
    binary_heap **out
);

/* Insert one element by copying elem_size bytes from element. */
NODISCARD status binary_heap_push(binary_heap *heap, const void *element);

/* Remove and return the top element pointer, or NULL when empty. */
void *binary_heap_pop(binary_heap *heap);

/* Return the top element pointer without removing it, or NULL when empty. */
const void *binary_heap_peek(const binary_heap *heap);

/* Return the number of stored elements. */
size_t binary_heap_size(const binary_heap *heap);

/* Return an iterator over heap backing storage order. */
iterator binary_heap_get_iterator(const binary_heap *heap);

/* Destroy and release owned storage. */
void binary_heap_destroy(binary_heap *heap);

#endif
