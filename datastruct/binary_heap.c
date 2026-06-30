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

#include "k4c/allocators/allocator.h"
#include "k4c/allocators/utils.h"
#include "k4c/assert.h"
#include "k4c/datastruct/binary_heap.h"
#include "k4c/datastruct/iterator.h"
#include "k4c/datastruct/vector.h"
#include "k4c/error.h"

struct k4c_binary_heap {
    k4c_vector *root;
    k4c_binary_heap_cmp_fn cmp;
    k4c_allocator k4c_allocator;
};

typedef struct k4c_binary_heap_iterator_state {
    const uint8_t *cursor;
    size_t elem_size;
    size_t remaining;
} k4c_binary_heap_iterator_state;

_Static_assert(
    sizeof(k4c_binary_heap_iterator_state) <= K4C_ITERATOR_STATE_SIZE,
    "k4c_binary_heap_iterator_state must fit in k4c_iterator"
);

static void k4c_binary_heap_swap_at(k4c_binary_heap *k4c_heap, size_t i, size_t j) {
    size_t elem_size = k4c_vector_elem_size(k4c_heap->root);
    void *a = k4c_vector_get(k4c_heap->root, i);
    void *b = k4c_vector_get(k4c_heap->root, j);

    k4c_memswap(a, b, elem_size);
}

static int k4c_binary_heap_compare_idx(const k4c_binary_heap *k4c_heap, size_t i, size_t j) {
    const void *a = k4c_vector_get(k4c_heap->root, i);
    const void *b = k4c_vector_get(k4c_heap->root, j);
    if (k4c_heap->cmp != NULL) {
        return k4c_heap->cmp(a, b);
    }
    return memcmp(a, b, k4c_vector_elem_size(k4c_heap->root));
}

static void k4c_binary_heap_sift_up(k4c_binary_heap *k4c_heap, size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        if (k4c_binary_heap_compare_idx(k4c_heap, idx, parent) >= 0) {
            break;
        }
        k4c_binary_heap_swap_at(k4c_heap, idx, parent);
        idx = parent;
    }
}

static void k4c_binary_heap_sift_down(k4c_binary_heap *k4c_heap, size_t idx) {
    size_t n = k4c_vector_size(k4c_heap->root);

    while (1) {
        size_t left = (2 * idx) + 1;
        size_t right = left + 1;
        size_t best = idx;

        if (left < n && k4c_binary_heap_compare_idx(k4c_heap, left, best) < 0) {
            best = left;
        }
        if (right < n && k4c_binary_heap_compare_idx(k4c_heap, right, best) < 0) {
            best = right;
        }
        if (best == idx) {
            break;
        }

        k4c_binary_heap_swap_at(k4c_heap, idx, best);
        idx = best;
    }
}

static const void *k4c_binary_heap_iterator_next(void *context) {
    K4C_ASSERT(context != NULL, "fatal: k4c_binary_heap_iterator_next invalid arguments");

    k4c_binary_heap_iterator_state *k4c_iterator = context;
    if (k4c_iterator->remaining == 0) {
        return NULL;
    }

    const void *item = k4c_iterator->cursor;
    k4c_iterator->cursor += k4c_iterator->elem_size;
    k4c_iterator->remaining -= 1;
    return item;
}

k4c_status k4c_binary_heap_create(
    size_t elem_size,
    k4c_binary_heap_cmp_fn cmp,
    k4c_allocator *k4c_allocator,
    k4c_binary_heap **out
) {
    K4C_ASSERT(elem_size > 0, "fatal: k4c_binary_heap_create invalid arguments");
    K4C_ASSERT(out != NULL, "fatal: k4c_binary_heap_create invalid arguments");

    *out = NULL;

    k4c_binary_heap *k4c_heap = NULL;
    k4c_status st = k4c_alloc(k4c_allocator, sizeof(*k4c_heap), (void **)&k4c_heap);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    k4c_heap->k4c_allocator = k4c_allocator_copy(k4c_allocator);

    st = k4c_vector_create(elem_size, &k4c_heap->k4c_allocator, &k4c_heap->root);
    if (st != K4C_STATUS_OK) {
        k4c_dealloc(k4c_allocator, k4c_heap);
        return st;
    }

    k4c_heap->cmp = cmp;

    *out = k4c_heap;
    return K4C_STATUS_OK;
}

k4c_status k4c_binary_heap_push(k4c_binary_heap *k4c_heap, const void *element) {
    K4C_ASSERT(k4c_heap != NULL, "fatal: k4c_binary_heap_push invalid arguments");
    K4C_ASSERT(element != NULL, "fatal: k4c_binary_heap_push invalid arguments");

    k4c_status st = k4c_vector_push(k4c_heap->root, element);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    k4c_binary_heap_sift_up(k4c_heap, k4c_vector_size(k4c_heap->root) - 1);
    return K4C_STATUS_OK;
}

void *k4c_binary_heap_pop(k4c_binary_heap *k4c_heap) {
    K4C_ASSERT(k4c_heap != NULL, "fatal: k4c_binary_heap_pop invalid arguments");
    size_t size = k4c_vector_size(k4c_heap->root);
    if (size == 0) {
        return NULL;
    }

    if (size == 1) {
        return k4c_vector_pop(k4c_heap->root);
    }

    k4c_binary_heap_swap_at(k4c_heap, 0, size - 1);
    void *out = k4c_vector_pop(k4c_heap->root);
    k4c_binary_heap_sift_down(k4c_heap, 0);

    return out;
}

const void *k4c_binary_heap_peek(const k4c_binary_heap *k4c_heap) {
    K4C_ASSERT(k4c_heap != NULL, "fatal: k4c_binary_heap_peek invalid arguments");
    if (k4c_vector_size(k4c_heap->root) == 0) {
        return NULL;
    }

    return k4c_vector_get(k4c_heap->root, 0);
}

size_t k4c_binary_heap_size(const k4c_binary_heap *k4c_heap) {
    K4C_ASSERT(k4c_heap != NULL, "fatal: k4c_binary_heap_size invalid arguments");

    return k4c_vector_size(k4c_heap->root);
}

k4c_iterator k4c_binary_heap_get_iterator(const k4c_binary_heap *k4c_heap) {
    K4C_ASSERT(k4c_heap != NULL, "fatal: k4c_binary_heap_get_iterator invalid arguments");

    k4c_iterator iter = k4c_iterator_from_state(k4c_binary_heap_iterator_next);
    k4c_binary_heap_iterator_state *state = k4c_iterator_state(&iter);
    state->cursor = (const uint8_t *)k4c_vector_data(k4c_heap->root);
    state->elem_size = k4c_vector_elem_size(k4c_heap->root);
    state->remaining = k4c_vector_size(k4c_heap->root);
    k4c_iterator_set_size_hint(&iter, k4c_vector_size(k4c_heap->root));
    return iter;
}

void k4c_binary_heap_destroy(k4c_binary_heap *k4c_heap) {
    K4C_ASSERT(k4c_heap != NULL, "fatal: k4c_binary_heap_destroy invalid arguments");
    k4c_allocator *k4c_allocator = &k4c_heap->k4c_allocator;
    k4c_vector_destroy(k4c_heap->root);
    k4c_dealloc(k4c_allocator, k4c_heap);
}
