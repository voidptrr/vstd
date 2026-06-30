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

#ifndef K4C_GENERAL_HEAP_H
#define K4C_GENERAL_HEAP_H

#include <stddef.h>

#include "k4c/allocators/allocator.h"
#include "k4c/error.h"

/*
 * Free-list k4c_heap memory model:
 *
 * +----------------------+----------------------+----------------------+----------------------+
 * | block header (A)     | payload (A)          | block header (B)     | payload (B)          |
 * +----------------------+----------------------+----------------------+----------------------+
 *
 * Each block header tracks:
 * - payload size
 * - free/used state
 * - intrusive doubly linked list node
 *
 * Linked-list view of blocks:
 *
 *   blocks -> [A free] <-> [B used] <-> [C free] -> NULL
 *
 * Allocation strategy:
 * - first-fit scan from head
 * - split large free blocks
 * - coalesce neighboring free blocks on k4c_dealloc
 */
typedef struct k4c_heap k4c_heap;

/*
 * Create a k4c_heap with `capacity` bytes of internal storage.
 */
k4c_status k4c_heap_create(size_t capacity, k4c_heap **out);

/*
 * Return the generic k4c_allocator view for this k4c_heap.
 */
k4c_allocator k4c_heap_allocator_view(k4c_heap *k4c_heap);

/* Total managed bytes (including internal metadata overhead). */
size_t k4c_heap_capacity(const k4c_heap *k4c_heap);

/* Sum of currently free payload bytes. */
size_t k4c_heap_available(const k4c_heap *k4c_heap);

/*
 * Destroy and release k4c_heap resources.
 */
void k4c_heap_destroy(k4c_heap *k4c_heap);

#endif
