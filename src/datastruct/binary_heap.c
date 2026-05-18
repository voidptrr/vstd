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

ckit_status ckit_binary_heap_init(ckit_binary_heap *heap, size_t elem_size, ckit_heap_cmp_fn cmp) {
    if (heap == NULL || cmp == NULL) {
        return CKIT_ERR_NULL;
    }
    if (elem_size == 0U) {
        return CKIT_ERR_RANGE;
    }

    ckit_status status = ckit_vector_init(&heap->root, elem_size, NULL);
    if (status != CKIT_OK) {
        return status;
    }
    heap->cmp = cmp;
    return CKIT_OK;
}

ckit_status ckit_binary_heap_push(ckit_binary_heap *heap, const void *element) {
    if (heap == NULL || element == NULL) {
        return CKIT_ERR_NULL;
    }

    ckit_status status = ckit_vector_push(&heap->root, element);
    if (status != CKIT_OK) {
        return status;
    }

    ckit_binary_heap_sift_up(heap, heap->root.size - 1U);
    return CKIT_OK;
}

ckit_status ckit_binary_heap_pop(ckit_binary_heap *heap, void *out) {
    if (heap == NULL || out == NULL) {
        return CKIT_ERR_NULL;
    }
    if (heap->root.size == 0U) {
        return CKIT_ERR_EMPTY;
    }

    size_t elem_size = heap->root.elem_size;
    uint8_t *base = (uint8_t *)heap->root.buffer;
    memcpy(out, base, elem_size);

    size_t last = heap->root.size - 1U;
    if (last > 0U) {
        memcpy(base, base + (last * elem_size), elem_size);
    }
    heap->root.size = last;

    if (heap->root.size > 0U) {
        ckit_binary_heap_sift_down(heap, 0U);
    }

    return CKIT_OK;
}

ckit_status ckit_binary_heap_peek(const ckit_binary_heap *heap, void *out) {
    if (heap == NULL || out == NULL) {
        return CKIT_ERR_NULL;
    }
    if (heap->root.size == 0U) {
        return CKIT_ERR_EMPTY;
    }

    memcpy(out, heap->root.buffer, heap->root.elem_size);
    return CKIT_OK;
}

ckit_status ckit_binary_heap_free(ckit_binary_heap *heap) {
    if (heap == NULL) {
        return CKIT_ERR_NULL;
    }
    ckit_status status = ckit_vector_free(&heap->root);
    if (status != CKIT_OK) {
        return status;
    }
    heap->cmp = NULL;
    return CKIT_OK;
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
