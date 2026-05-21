#include <stdint.h>
#include <string.h>

#include "ckit/common/panic.h"
#include "ckit/datastruct/binary_heap.h"
#include "ckit/datastruct/vector.h"
#include "ckit/memory/allocators/allocator.h"
#include "ckit/memory/bytes.h"

struct ckit_binary_heap {
    ckit_vector *root;
    ckit_heap_cmp_fn cmp;
    ckit_allocator *allocator;
};

static void ckit_binary_heap_swap_at(ckit_binary_heap *heap, size_t i, size_t j) {
    size_t elem_size = ckit_vector_elem_size(heap->root);
    void *a = ckit_vector_get(heap->root, i);
    void *b = ckit_vector_get(heap->root, j);

    ckit_memswap(a, b, elem_size);
}

static int ckit_binary_heap_compare_idx(const ckit_binary_heap *heap, size_t i, size_t j) {
    const void *a = ckit_vector_get_const(heap->root, i);
    const void *b = ckit_vector_get_const(heap->root, j);
    return heap->cmp(a, b);
}

static void ckit_binary_heap_sift_up(ckit_binary_heap *heap, size_t idx) {
    while (idx > 0U) {
        size_t parent = (idx - 1U) / 2U;
        if (ckit_binary_heap_compare_idx(heap, idx, parent) >= 0) {
            break;
        }
        ckit_binary_heap_swap_at(heap, idx, parent);
        idx = parent;
    }
}

static void ckit_binary_heap_sift_down(ckit_binary_heap *heap, size_t idx) {
    size_t n = ckit_vector_size(heap->root);

    while (1) {
        size_t left = (2U * idx) + 1U;
        size_t right = left + 1U;
        size_t best = idx;

        if (left < n && ckit_binary_heap_compare_idx(heap, left, best) < 0) {
            best = left;
        }
        if (right < n && ckit_binary_heap_compare_idx(heap, right, best) < 0) {
            best = right;
        }
        if (best == idx) {
            break;
        }

        ckit_binary_heap_swap_at(heap, idx, best);
        idx = best;
    }
}

ckit_binary_heap *ckit_binary_heap_init(size_t elem_size, ckit_heap_cmp_fn cmp,
                                        ckit_allocator *allocator) {
    CKIT_ASSERT(cmp != NULL, "fatal: ckit_binary_heap_init invalid arguments");
    CKIT_ASSERT(elem_size > 0U, "fatal: ckit_binary_heap_init invalid arguments");

    ckit_binary_heap *heap = ckit_malloc(allocator, sizeof(*heap));
    heap->root = ckit_vector_init(elem_size, allocator);
    heap->cmp = cmp;
    heap->allocator = allocator;

    return heap;
}

void ckit_binary_heap_push(ckit_binary_heap *heap, const void *element) {
    CKIT_ASSERT(heap != NULL, "fatal: ckit_binary_heap_push invalid arguments");
    CKIT_ASSERT(element != NULL, "fatal: ckit_binary_heap_push invalid arguments");

    ckit_vector_push(heap->root, element);

    ckit_binary_heap_sift_up(heap, ckit_vector_size(heap->root) - 1U);
}

void *ckit_binary_heap_pop(ckit_binary_heap *heap) {
    CKIT_ASSERT(heap != NULL, "fatal: ckit_binary_heap_pop invalid arguments");
    size_t size = ckit_vector_size(heap->root);
    if (size == 0U) {
        return NULL;
    }

    void *out = ckit_vector_swap_remove(heap->root, 0U);

    if (size > 1U) {
        ckit_binary_heap_sift_down(heap, 0U);
    }

    return out;
}

const void *ckit_binary_heap_peek(const ckit_binary_heap *heap) {
    CKIT_ASSERT(heap != NULL, "fatal: ckit_binary_heap_peek invalid arguments");
    if (ckit_vector_size(heap->root) == 0U) {
        return NULL;
    }

    return ckit_vector_get_const(heap->root, 0U);
}

void ckit_binary_heap_free(ckit_binary_heap *heap) {
    CKIT_ASSERT(heap != NULL, "fatal: ckit_binary_heap_free invalid arguments");
    ckit_allocator *allocator = heap->allocator;
    ckit_vector_free(heap->root);
    ckit_dealloc(allocator, heap);
}

size_t ckit_binary_heap_size(const ckit_binary_heap *heap) {
    if (heap == NULL) {
        return 0U;
    }
    return ckit_vector_size(heap->root);
}

bool ckit_binary_heap_is_empty(const ckit_binary_heap *heap) {
    return ckit_binary_heap_size(heap) == 0U;
}
