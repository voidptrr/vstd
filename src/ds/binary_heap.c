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

#include <stdint.h>
#include <string.h>

#include "vstd/assert.h"
#include "vstd/ds/binary_heap.h"
#include "vstd/ds/iterator.h"
#include "vstd/ds/vector.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/utils.h"

struct binary_heap {
    vector *root;
    binary_heap_cmp_fn cmp;
    allocator *allocator;
};

typedef struct binary_heap_iterator_state {
    const uint8_t *cursor;
    size_t elem_size;
    size_t remaining;
} binary_heap_iterator_state;

_Static_assert(
    sizeof(binary_heap_iterator_state) <= ITERATOR_STATE_SIZE,
    "binary_heap_iterator_state must fit in iterator"
);

static void binary_heap_swap_at(binary_heap *heap, size_t i, size_t j) {
    size_t elem_size = vector_elem_size(heap->root);
    void *a = vector_get(heap->root, i);
    void *b = vector_get(heap->root, j);

    memswap(a, b, elem_size);
}

static int binary_heap_compare_idx(const binary_heap *heap, size_t i, size_t j) {
    const void *a = vector_get_const(heap->root, i);
    const void *b = vector_get_const(heap->root, j);
    if (heap->cmp != NULL) {
        return heap->cmp(a, b);
    }
    return memcmp(a, b, vector_elem_size(heap->root));
}

static void binary_heap_sift_up(binary_heap *heap, size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        if (binary_heap_compare_idx(heap, idx, parent) >= 0) {
            break;
        }
        binary_heap_swap_at(heap, idx, parent);
        idx = parent;
    }
}

static void binary_heap_sift_down(binary_heap *heap, size_t idx) {
    size_t n = vector_size(heap->root);

    while (1) {
        size_t left = (2 * idx) + 1;
        size_t right = left + 1;
        size_t best = idx;

        if (left < n && binary_heap_compare_idx(heap, left, best) < 0) {
            best = left;
        }
        if (right < n && binary_heap_compare_idx(heap, right, best) < 0) {
            best = right;
        }
        if (best == idx) {
            break;
        }

        binary_heap_swap_at(heap, idx, best);
        idx = best;
    }
}

static const void *binary_heap_iterator_next(void *context) {
    ASSERT(context != NULL, "fatal: binary_heap_iterator_next invalid arguments");

    binary_heap_iterator_state *iterator = context;
    if (iterator->remaining == 0) {
        return NULL;
    }

    const void *item = iterator->cursor;
    iterator->cursor += iterator->elem_size;
    iterator->remaining -= 1;
    return item;
}

status binary_heap_create(
    size_t elem_size,
    binary_heap_cmp_fn cmp,
    allocator *allocator,
    binary_heap **out
) {
    ASSERT(elem_size > 0, "fatal: binary_heap_create invalid arguments");
    ASSERT(out != NULL, "fatal: binary_heap_create invalid arguments");

    *out = NULL;

    binary_heap *heap = NULL;
    status st = alloc(allocator, sizeof(*heap), (void **)&heap);
    if (st != STATUS_OK) {
        return st;
    }

    st = vector_create(elem_size, allocator, &heap->root);
    if (st != STATUS_OK) {
        dealloc(allocator, heap);
        return st;
    }

    heap->cmp = cmp;
    heap->allocator = allocator;

    *out = heap;
    return STATUS_OK;
}

status binary_heap_push(binary_heap *heap, const void *element) {
    ASSERT(heap != NULL, "fatal: binary_heap_push invalid arguments");
    ASSERT(element != NULL, "fatal: binary_heap_push invalid arguments");

    status st = vector_push(heap->root, element);
    if (st != STATUS_OK) {
        return st;
    }

    binary_heap_sift_up(heap, vector_size(heap->root) - 1);
    return STATUS_OK;
}

void *binary_heap_pop(binary_heap *heap) {
    ASSERT(heap != NULL, "fatal: binary_heap_pop invalid arguments");
    size_t size = vector_size(heap->root);
    if (size == 0) {
        return NULL;
    }

    if (size == 1) {
        return vector_pop(heap->root);
    }

    binary_heap_swap_at(heap, 0, size - 1);
    void *out = vector_pop(heap->root);
    binary_heap_sift_down(heap, 0);

    return out;
}

const void *binary_heap_peek(const binary_heap *heap) {
    ASSERT(heap != NULL, "fatal: binary_heap_peek invalid arguments");
    if (vector_size(heap->root) == 0) {
        return NULL;
    }

    return vector_get_const(heap->root, 0);
}

size_t binary_heap_size(const binary_heap *heap) {
    ASSERT(heap != NULL, "fatal: binary_heap_size invalid arguments");

    return vector_size(heap->root);
}

iterator binary_heap_get_iterator(const binary_heap *heap) {
    ASSERT(heap != NULL, "fatal: binary_heap_get_iterator invalid arguments");

    iterator iter = iterator_from_state(binary_heap_iterator_next);
    binary_heap_iterator_state *state = iterator_state(&iter);
    state->cursor = (const uint8_t *)vector_data_const(heap->root);
    state->elem_size = vector_elem_size(heap->root);
    state->remaining = vector_size(heap->root);
    iterator_set_size_hint(&iter, vector_size(heap->root));
    return iter;
}

void binary_heap_destroy(binary_heap *heap) {
    ASSERT(heap != NULL, "fatal: binary_heap_destroy invalid arguments");
    allocator *allocator = heap->allocator;
    vector_destroy(heap->root);
    dealloc(allocator, heap);
}
