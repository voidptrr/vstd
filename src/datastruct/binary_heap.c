#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "ckit/datastruct/binary_heap.h"
#include "ckit/mem/bytes.h"

static void ckit_binary_heap_swap_at(ckit_binary_heap *heap, size_t i, size_t j) {
    size_t elem_size = heap->root.elem_size;
    uint8_t *base = (uint8_t *)heap->root.buffer;
    uint8_t *a = base + (i * elem_size);
    uint8_t *b = base + (j * elem_size);

    ckit_memswap(a, b, elem_size);
}

static int ckit_binary_heap_compare_idx(const ckit_binary_heap *heap, size_t i, size_t j) {
    size_t elem_size = heap->root.elem_size;
    uint8_t *base = (uint8_t *)heap->root.buffer;
    const void *a = base + (i * elem_size);
    const void *b = base + (j * elem_size);
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
    size_t n = heap->root.size;

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

void ckit_binary_heap_init(ckit_binary_heap *heap, size_t elem_size, ckit_heap_cmp_fn cmp,
                           ckit_allocator *allocator) {
    assert(heap != NULL);
    assert(cmp != NULL);
    assert(elem_size > 0U);

    ckit_vector_init(&heap->root, elem_size, allocator);
    heap->cmp = cmp;
}

void ckit_binary_heap_push(ckit_binary_heap *heap, const void *element) {
    assert(heap != NULL);
    assert(element != NULL);

    ckit_vector_push(&heap->root, element);

    ckit_binary_heap_sift_up(heap, heap->root.size - 1U);
}

void *ckit_binary_heap_pop(ckit_binary_heap *heap) {
    assert(heap != NULL);
    if (heap->root.size == 0U) {
        return NULL;
    }

    size_t elem_size = heap->root.elem_size;
    uint8_t *base = (uint8_t *)heap->root.buffer;
    void *out = base;

    size_t last = heap->root.size - 1U;
    if (last > 0U) {
        memcpy(base, base + (last * elem_size), elem_size);
    }
    heap->root.size = last;

    if (heap->root.size > 0U) {
        ckit_binary_heap_sift_down(heap, 0U);
    }

    return out;
}

const void *ckit_binary_heap_peek(const ckit_binary_heap *heap) {
    assert(heap != NULL);
    if (heap->root.size == 0U) {
        return NULL;
    }

    return heap->root.buffer;
}

void ckit_binary_heap_free(ckit_binary_heap *heap) {
    assert(heap != NULL);
    ckit_vector_free(&heap->root);
    heap->cmp = NULL;
}

size_t ckit_binary_heap_size(const ckit_binary_heap *heap) {
    if (heap == NULL) {
        return 0U;
    }
    return ckit_vector_size(&heap->root);
}

bool ckit_binary_heap_is_empty(const ckit_binary_heap *heap) {
    return ckit_binary_heap_size(heap) == 0U;
}
