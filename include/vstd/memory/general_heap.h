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

#ifndef VSTD_GENERAL_HEAP_H
#define VSTD_GENERAL_HEAP_H

#include <stddef.h>

#include "vstd/error.h"
#include "vstd/memory/allocator.h"

/*
 * Free-list heap memory model:
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
 * - coalesce neighboring free blocks on dealloc
 */
typedef struct vs_heap vs_heap;

/*
 * Create a heap with `capacity` bytes of internal storage.
 */
VS_NODISCARD vs_status vs_heap_create(size_t capacity, vs_heap **out);

/*
 * Return the generic allocator view owned by this heap.
 */
vs_allocator *vs_heap_allocator(vs_heap *heap);

/*
 * Allocate `size` bytes from heap.
 * Returns NULL when no suitable free block exists.
 */
void *vs_heap_alloc(vs_heap *heap, size_t size);

/*
 * Free a pointer previously returned by vs_heap_alloc/vs_heap_realloc.
 */
void vs_heap_dealloc(vs_heap *heap, void *ptr);

/*
 * Resize an existing heap allocation.
 * Behaves like realloc: NULL ptr allocates, size 0 frees and returns NULL.
 */
void *vs_heap_realloc(vs_heap *heap, void *ptr, size_t size);

/* Total managed bytes (including internal metadata overhead). */
size_t vs_heap_capacity(const vs_heap *heap);

/* Sum of currently free payload bytes. */
size_t vs_heap_available(const vs_heap *heap);

/*
 * Destroy and release heap resources.
 */
void vs_heap_destroy(vs_heap *heap);

#endif
