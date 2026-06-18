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

#ifndef VSTD_BINARY_HEAP_H
#define VSTD_BINARY_HEAP_H

#include <stdbool.h>
#include <stddef.h>

#include "vstd/memory/allocators/allocator.h"

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
typedef int (*vs_binary_heap_cmp_fn)(const void *lhs, const void *rhs);

typedef struct vs_binary_heap vs_binary_heap;

/* Create a heap for elements of size elem_size. NULL cmp uses byte-wise ordering. */
vs_binary_heap *vs_binary_heap_create(
    size_t elem_size,
    vs_binary_heap_cmp_fn cmp,
    vs_allocator *allocator
);

/* Insert one element by copying elem_size bytes from element. */
void vs_binary_heap_push(vs_binary_heap *heap, const void *element);

/* Remove and return the top element pointer, or NULL when empty. */
void *vs_binary_heap_pop(vs_binary_heap *heap);

/* Return the top element pointer without removing it, or NULL when empty. */
const void *vs_binary_heap_peek(const vs_binary_heap *heap);

/* Return the number of stored elements. */
size_t vs_binary_heap_size(const vs_binary_heap *heap);

/* Destroy and release owned storage. */
void vs_binary_heap_destroy(vs_binary_heap *heap);

#endif
