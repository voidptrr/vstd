#include <stdint.h>
#include <string.h>

#include "cstd/datastruct/binary_heap.h"

static void cstd_binary_heap_swap_at(cstd_binary_heap *heap, size_t i, size_t j) {
    size_t elem_size = heap->root.elem_size;
    uint8_t *base = (uint8_t *)heap->root.buffer;
    uint8_t *a = base + (i * elem_size);
    uint8_t *b = base + (j * elem_size);

    for (size_t k = 0; k < elem_size; k++) {
        uint8_t tmp = a[k];
        a[k] = b[k];
        b[k] = tmp;
    }
}

static int cstd_binary_heap_compare_idx(const cstd_binary_heap *heap, size_t i, size_t j) {
    size_t elem_size = heap->root.elem_size;
    uint8_t *base = (uint8_t *)heap->root.buffer;
    const void *a = base + (i * elem_size);
    const void *b = base + (j * elem_size);
    return heap->cmp(a, b);
}

static void cstd_binary_heap_sift_up(cstd_binary_heap *heap, size_t idx) {
    while (idx > 0U) {
        size_t parent = (idx - 1U) / 2U;
        if (cstd_binary_heap_compare_idx(heap, idx, parent) >= 0) {
            break;
        }
        cstd_binary_heap_swap_at(heap, idx, parent);
        idx = parent;
    }
}

static void cstd_binary_heap_sift_down(cstd_binary_heap *heap, size_t idx) {
    size_t n = heap->root.size;

    while (1) {
        size_t left = (2U * idx) + 1U;
        size_t right = left + 1U;
        size_t best = idx;

        if (left < n && cstd_binary_heap_compare_idx(heap, left, best) < 0) {
            best = left;
        }
        if (right < n && cstd_binary_heap_compare_idx(heap, right, best) < 0) {
            best = right;
        }
        if (best == idx) {
            break;
        }

        cstd_binary_heap_swap_at(heap, idx, best);
        idx = best;
    }
}

cstd_status cstd_binary_heap_init(cstd_binary_heap *heap, size_t elem_size, cstd_heap_cmp_fn cmp) {
    if (heap == NULL || cmp == NULL) {
        return CSTD_ERR_NULL;
    }
    if (elem_size == 0U) {
        return CSTD_ERR_RANGE;
    }

    cstd_status status = cstd_vector_init(&heap->root, elem_size);
    if (status != CSTD_OK) {
        return status;
    }
    heap->cmp = cmp;
    return CSTD_OK;
}

cstd_status cstd_binary_heap_push(cstd_binary_heap *heap, const void *element) {
    if (heap == NULL || element == NULL) {
        return CSTD_ERR_NULL;
    }

    cstd_status status = cstd_vector_push(&heap->root, element);
    if (status != CSTD_OK) {
        return status;
    }

    cstd_binary_heap_sift_up(heap, heap->root.size - 1U);
    return CSTD_OK;
}

cstd_status cstd_binary_heap_pop(cstd_binary_heap *heap, void *out) {
    if (heap == NULL || out == NULL) {
        return CSTD_ERR_NULL;
    }
    if (heap->root.size == 0U) {
        return CSTD_ERR_EMPTY;
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
        cstd_binary_heap_sift_down(heap, 0U);
    }

    return CSTD_OK;
}

cstd_status cstd_binary_heap_peek(const cstd_binary_heap *heap, void *out) {
    if (heap == NULL || out == NULL) {
        return CSTD_ERR_NULL;
    }
    if (heap->root.size == 0U) {
        return CSTD_ERR_EMPTY;
    }

    memcpy(out, heap->root.buffer, heap->root.elem_size);
    return CSTD_OK;
}

cstd_status cstd_binary_heap_free(cstd_binary_heap *heap) {
    if (heap == NULL) {
        return CSTD_ERR_NULL;
    }
    cstd_status status = cstd_vector_free(&heap->root);
    if (status != CSTD_OK) {
        return status;
    }
    heap->cmp = NULL;
    return CSTD_OK;
}

size_t cstd_binary_heap_size(const cstd_binary_heap *heap) {
    if (heap == NULL) {
        return 0U;
    }
    return cstd_vector_size(&heap->root);
}

bool cstd_binary_heap_is_empty(const cstd_binary_heap *heap) {
    return cstd_binary_heap_size(heap) == 0U;
}
