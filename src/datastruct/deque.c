#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ckit/datastruct/deque.h"
#include "ckit/mem/allocators/allocator.h"

#define CKIT_DEQUE_DEFAULT_CAPACITY 16

static void *ckit_deque_alloc(const ckit_deque *deque, size_t size) {
    if (deque->allocator == NULL || deque->allocator->alloc == NULL) {
        return ckit_malloc(size);
    }
    return deque->allocator->alloc(deque->allocator->ctx, size);
}

static void ckit_deque_dealloc(const ckit_deque *deque, void *ptr) {
    if (deque->allocator == NULL || deque->allocator->dealloc == NULL) {
        free(ptr);
        return;
    }
    deque->allocator->dealloc(deque->allocator->ctx, ptr);
}

static ckit_status ckit_deque_grow(ckit_deque *deque) {
    size_t old_capacity = deque->capacity;
    size_t new_capacity = old_capacity * 2;
    uint8_t *old_buffer = (uint8_t *)deque->buffer;
    uint8_t *new_buffer = ckit_deque_alloc(deque, new_capacity * deque->elem_size);
    if (new_buffer == NULL) {
        return CKIT_ERR_RANGE;
    }

    if (deque->size > 0) {
        if (deque->head < deque->tail) {
            memcpy(new_buffer, old_buffer + (deque->head * deque->elem_size),
                   deque->size * deque->elem_size);
        } else {
            size_t first_count = old_capacity - deque->head;
            size_t second_count = deque->tail;

            memcpy(new_buffer, old_buffer + (deque->head * deque->elem_size),
                   first_count * deque->elem_size);
            if (second_count > 0) {
                memcpy(new_buffer + (first_count * deque->elem_size), old_buffer,
                       second_count * deque->elem_size);
            }
        }
    }

    ckit_deque_dealloc(deque, deque->buffer);
    deque->buffer = new_buffer;
    deque->capacity = new_capacity;
    deque->head = 0;
    deque->tail = deque->size;

    return CKIT_OK;
}

ckit_status ckit_deque_init(ckit_deque *deque, size_t elem_size, ckit_allocator *allocator) {
    if (deque == NULL) {
        return CKIT_ERR_NULL;
    }

    if (elem_size == 0) {
        return CKIT_ERR_RANGE;
    }

    deque->allocator = allocator;

    void *buffer = ckit_deque_alloc(deque, elem_size * CKIT_DEQUE_DEFAULT_CAPACITY);
    if (buffer == NULL) {
        return CKIT_ERR_RANGE;
    }

    deque->size = 0;
    deque->elem_size = elem_size;
    deque->capacity = CKIT_DEQUE_DEFAULT_CAPACITY;
    deque->head = 0;
    deque->tail = 0;
    deque->buffer = buffer;

    return CKIT_OK;
}

ckit_status ckit_deque_push(ckit_deque *deque, const void *element) {
    if (deque == NULL || element == NULL) {
        return CKIT_ERR_NULL;
    }

    if (deque->capacity == 0U) {
        return CKIT_ERR_RANGE;
    }

    if (deque->size == deque->capacity) {
        ckit_status status = ckit_deque_grow(deque);
        if (status != CKIT_OK) {
            return status;
        }
    }

    uint8_t *base = (uint8_t *)deque->buffer;
    void *dst = base + (deque->tail * deque->elem_size);
    memcpy(dst, element, deque->elem_size);
    deque->size += 1;
    deque->tail = (deque->tail + 1) % deque->capacity;

    return CKIT_OK;
}

ckit_status ckit_deque_pushfront(ckit_deque *deque, const void *element) {
    if (deque == NULL || element == NULL) {
        return CKIT_ERR_NULL;
    }

    if (deque->capacity == 0U) {
        return CKIT_ERR_RANGE;
    }

    if (deque->size == deque->capacity) {
        ckit_status status = ckit_deque_grow(deque);
        if (status != CKIT_OK) {
            return status;
        }
    }

    deque->head = (deque->head + deque->capacity - 1) % deque->capacity;

    uint8_t *base = (uint8_t *)deque->buffer;
    void *dst = base + (deque->head * deque->elem_size);
    memcpy(dst, element, deque->elem_size);
    deque->size += 1;

    return CKIT_OK;
}

ckit_status ckit_deque_popleft(ckit_deque *deque, void *out) {
    if (deque == NULL || out == NULL) {
        return CKIT_ERR_NULL;
    }

    if (deque->capacity == 0U) {
        return CKIT_ERR_RANGE;
    }

    if (deque->size == 0) {
        return CKIT_ERR_EMPTY;
    }

    uint8_t *base = (uint8_t *)deque->buffer;
    void *src = base + (deque->head * deque->elem_size);
    memcpy(out, src, deque->elem_size);

    deque->head = (deque->head + 1) % deque->capacity;
    deque->size -= 1;

    return CKIT_OK;
}

ckit_status ckit_deque_popback(ckit_deque *deque, void *out) {
    if (deque == NULL || out == NULL) {
        return CKIT_ERR_NULL;
    }

    if (deque->capacity == 0U) {
        return CKIT_ERR_RANGE;
    }

    if (deque->size == 0) {
        return CKIT_ERR_EMPTY;
    }

    deque->tail = (deque->tail + deque->capacity - 1) % deque->capacity;

    uint8_t *base = (uint8_t *)deque->buffer;
    void *src = base + (deque->tail * deque->elem_size);
    memcpy(out, src, deque->elem_size);

    deque->size -= 1;

    return CKIT_OK;
}

ckit_status ckit_deque_peekleft(const ckit_deque *deque, void *out) {
    if (deque == NULL || out == NULL) {
        return CKIT_ERR_NULL;
    }

    if (deque->size == 0) {
        return CKIT_ERR_EMPTY;
    }

    uint8_t *base = (uint8_t *)deque->buffer;
    void *src = base + (deque->head * deque->elem_size);
    memcpy(out, src, deque->elem_size);

    return CKIT_OK;
}

ckit_status ckit_deque_peekback(const ckit_deque *deque, void *out) {
    if (deque == NULL || out == NULL) {
        return CKIT_ERR_NULL;
    }

    if (deque->capacity == 0U) {
        return CKIT_ERR_RANGE;
    }

    if (deque->size == 0) {
        return CKIT_ERR_EMPTY;
    }

    size_t last_index = (deque->tail + deque->capacity - 1) % deque->capacity;
    uint8_t *base = (uint8_t *)deque->buffer;
    void *src = base + (last_index * deque->elem_size);
    memcpy(out, src, deque->elem_size);

    return CKIT_OK;
}

ckit_status ckit_deque_free(ckit_deque *deque) {
    if (deque == NULL) {
        return CKIT_ERR_NULL;
    }

    ckit_deque_dealloc(deque, deque->buffer);
    deque->buffer = NULL;
    deque->size = 0;
    deque->capacity = 0;
    deque->head = 0;
    deque->tail = 0;
    deque->allocator = NULL;

    return CKIT_OK;
}

size_t ckit_deque_size(const ckit_deque *deque) {
    if (deque == NULL) {
        return 0;
    }

    return deque->size;
}

bool ckit_deque_is_empty(const ckit_deque *deque) {
    return ckit_deque_size(deque) == 0;
}
