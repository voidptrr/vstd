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

#include <string.h>

#include "vstd/assert.h"
#include "vstd/datastruct/binary_heap.h"
#include "vstd/datastruct/vector.h"
#include "vstd/memory/allocators/allocator.h"
#include "vstd/memory/utils.h"

struct vs_binary_heap {
    vs_vector *root;
    vs_binary_heap_cmp_fn cmp;
    vs_allocator *allocator;
};

static void vs_binary_heap_swap_at(vs_binary_heap *heap, size_t i, size_t j) {
    size_t elem_size = vs_vector_elem_size(heap->root);
    void *a = vs_vector_get(heap->root, i);
    void *b = vs_vector_get(heap->root, j);

    vs_memswap(a, b, elem_size);
}

static int vs_binary_heap_compare_idx(const vs_binary_heap *heap, size_t i, size_t j) {
    const void *a = vs_vector_get_const(heap->root, i);
    const void *b = vs_vector_get_const(heap->root, j);
    if (heap->cmp != NULL) {
        return heap->cmp(a, b);
    }
    return memcmp(a, b, vs_vector_elem_size(heap->root));
}

static void vs_binary_heap_sift_up(vs_binary_heap *heap, size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        if (vs_binary_heap_compare_idx(heap, idx, parent) >= 0) {
            break;
        }
        vs_binary_heap_swap_at(heap, idx, parent);
        idx = parent;
    }
}

static void vs_binary_heap_sift_down(vs_binary_heap *heap, size_t idx) {
    size_t n = vs_vector_size(heap->root);

    while (1) {
        size_t left = (2 * idx) + 1;
        size_t right = left + 1;
        size_t best = idx;

        if (left < n && vs_binary_heap_compare_idx(heap, left, best) < 0) {
            best = left;
        }
        if (right < n && vs_binary_heap_compare_idx(heap, right, best) < 0) {
            best = right;
        }
        if (best == idx) {
            break;
        }

        vs_binary_heap_swap_at(heap, idx, best);
        idx = best;
    }
}

vs_binary_heap *vs_binary_heap_create(
    size_t elem_size,
    vs_binary_heap_cmp_fn cmp,
    vs_allocator *allocator
) {
    VSTD_ASSERT(elem_size > 0, "fatal: vs_binary_heap_create invalid arguments");

    vs_binary_heap *heap = vs_malloc(allocator, sizeof(vs_binary_heap));
    heap->root = vs_vector_create(elem_size, allocator);
    heap->cmp = cmp;
    heap->allocator = allocator;

    return heap;
}

void vs_binary_heap_push(vs_binary_heap *heap, const void *element) {
    VSTD_ASSERT(heap != NULL, "fatal: vs_binary_heap_push invalid arguments");
    VSTD_ASSERT(element != NULL, "fatal: vs_binary_heap_push invalid arguments");

    vs_vector_push(heap->root, element);

    vs_binary_heap_sift_up(heap, vs_vector_size(heap->root) - 1);
}

void *vs_binary_heap_pop(vs_binary_heap *heap) {
    VSTD_ASSERT(heap != NULL, "fatal: vs_binary_heap_pop invalid arguments");
    size_t size = vs_vector_size(heap->root);
    if (size == 0) {
        return NULL;
    }

    void *out = vs_vector_swap_remove(heap->root, 0);

    if (size > 1) {
        vs_binary_heap_sift_down(heap, 0);
    }

    return out;
}

const void *vs_binary_heap_peek(const vs_binary_heap *heap) {
    VSTD_ASSERT(heap != NULL, "fatal: vs_binary_heap_peek invalid arguments");
    if (vs_vector_size(heap->root) == 0) {
        return NULL;
    }

    return vs_vector_get_const(heap->root, 0);
}

size_t vs_binary_heap_size(const vs_binary_heap *heap) {
    VSTD_ASSERT(heap != NULL, "fatal: vs_binary_heap_size invalid arguments");

    return vs_vector_size(heap->root);
}

void vs_binary_heap_destroy(vs_binary_heap *heap) {
    VSTD_ASSERT(heap != NULL, "fatal: vs_binary_heap_destroy invalid arguments");
    vs_allocator *allocator = heap->allocator;
    vs_vector_destroy(heap->root);
    vs_dealloc(allocator, heap);
}
