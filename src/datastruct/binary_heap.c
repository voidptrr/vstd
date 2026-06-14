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

#include "ckit/common/panic.h"
#include "ckit/datastruct/binary_heap.h"
#include "ckit/datastruct/vector.h"
#include "ckit/memory/allocators/allocator.h"
#include "ckit/memory/bytes.h"

struct ck_binary_heap {
    ck_vector *root;
    ck_heap_cmp_fn cmp;
    ck_allocator *allocator;
};

static void ck_binary_heap_swap_at(ck_binary_heap *heap, size_t i, size_t j) {
    size_t elem_size = ck_vector_elem_size(heap->root);
    void *a = ck_vector_get(heap->root, i);
    void *b = ck_vector_get(heap->root, j);

    ck_memswap(a, b, elem_size);
}

static int ck_binary_heap_compare_idx(const ck_binary_heap *heap, size_t i, size_t j) {
    const void *a = ck_vector_get_const(heap->root, i);
    const void *b = ck_vector_get_const(heap->root, j);
    return heap->cmp(a, b);
}

static void ck_binary_heap_sift_up(ck_binary_heap *heap, size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        if (ck_binary_heap_compare_idx(heap, idx, parent) >= 0) {
            break;
        }
        ck_binary_heap_swap_at(heap, idx, parent);
        idx = parent;
    }
}

static void ck_binary_heap_sift_down(ck_binary_heap *heap, size_t idx) {
    size_t n = ck_vector_size(heap->root);

    while (1) {
        size_t left = (2 * idx) + 1;
        size_t right = left + 1;
        size_t best = idx;

        if (left < n && ck_binary_heap_compare_idx(heap, left, best) < 0) {
            best = left;
        }
        if (right < n && ck_binary_heap_compare_idx(heap, right, best) < 0) {
            best = right;
        }
        if (best == idx) {
            break;
        }

        ck_binary_heap_swap_at(heap, idx, best);
        idx = best;
    }
}

ck_binary_heap *ck_binary_heap_create(size_t elem_size, ck_heap_cmp_fn cmp,
                                      ck_allocator *allocator) {
    CK_ASSERT(cmp != NULL, "fatal: ck_binary_heap_create invalid arguments");
    CK_ASSERT(elem_size > 0, "fatal: ck_binary_heap_create invalid arguments");

    ck_binary_heap *heap = ck_malloc(allocator, sizeof(ck_binary_heap));
    heap->root = ck_vector_create(elem_size, allocator);
    heap->cmp = cmp;
    heap->allocator = allocator;

    return heap;
}

void ck_binary_heap_push(ck_binary_heap *heap, const void *element) {
    CK_ASSERT(heap != NULL, "fatal: ck_binary_heap_push invalid arguments");
    CK_ASSERT(element != NULL, "fatal: ck_binary_heap_push invalid arguments");

    ck_vector_push(heap->root, element);

    ck_binary_heap_sift_up(heap, ck_vector_size(heap->root) - 1);
}

void *ck_binary_heap_pop(ck_binary_heap *heap) {
    CK_ASSERT(heap != NULL, "fatal: ck_binary_heap_pop invalid arguments");
    size_t size = ck_vector_size(heap->root);
    if (size == 0) {
        return NULL;
    }

    void *out = ck_vector_swap_remove(heap->root, 0);

    if (size > 1) {
        ck_binary_heap_sift_down(heap, 0);
    }

    return out;
}

const void *ck_binary_heap_peek(const ck_binary_heap *heap) {
    CK_ASSERT(heap != NULL, "fatal: ck_binary_heap_peek invalid arguments");
    if (ck_vector_size(heap->root) == 0) {
        return NULL;
    }

    return ck_vector_get_const(heap->root, 0);
}

size_t ck_binary_heap_size(const ck_binary_heap *heap) {
    CK_ASSERT(heap != NULL, "fatal: ck_binary_heap_size invalid arguments");

    return ck_vector_size(heap->root);
}

void ck_binary_heap_destroy(ck_binary_heap *heap) {
    CK_ASSERT(heap != NULL, "fatal: ck_binary_heap_destroy invalid arguments");
    ck_allocator *allocator = heap->allocator;
    ck_vector_destroy(heap->root);
    ck_dealloc(allocator, heap);
}
