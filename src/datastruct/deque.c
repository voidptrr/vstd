#include <stdint.h>
#include <string.h>

#include "ckit/common/panic.h"
#include "ckit/datastruct/deque.h"
#include "ckit/memory/allocators/allocator.h"

#define CKIT_DEQUE_DEFAULT_CAPACITY 16

struct ckit_deque {
    size_t size;
    size_t elem_size;
    size_t capacity;
    size_t head;
    size_t tail;
    void *buffer;
    ckit_allocator *allocator;
};

static void ckit_deque_grow(ckit_deque *deque) {
    size_t old_capacity = deque->capacity;
    size_t new_capacity = old_capacity * 2;
    uint8_t *old_buffer = (uint8_t *)deque->buffer;

    size_t alloc_size = new_capacity * deque->elem_size;
    uint8_t *new_buffer = ckit_malloc(deque->allocator, alloc_size);

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

    ckit_dealloc(deque->allocator, deque->buffer);
    deque->buffer = new_buffer;
    deque->capacity = new_capacity;
    deque->head = 0;
    deque->tail = deque->size;
}

ckit_deque *ckit_deque_init(size_t elem_size, ckit_allocator *allocator) {
    CKIT_ASSERT(elem_size > 0U, "fatal: ckit_deque_init invalid arguments");

    ckit_deque *deque = ckit_malloc(allocator, sizeof(*deque));
    deque->allocator = allocator;

    size_t alloc_size = elem_size * CKIT_DEQUE_DEFAULT_CAPACITY;
    void *buffer = ckit_malloc(allocator, alloc_size);

    deque->size = 0;
    deque->elem_size = elem_size;
    deque->capacity = CKIT_DEQUE_DEFAULT_CAPACITY;
    deque->head = 0;
    deque->tail = 0;
    deque->buffer = buffer;

    return deque;
}

void ckit_deque_push(ckit_deque *deque, const void *element) {
    CKIT_ASSERT(deque != NULL, "fatal: ckit_deque_push invalid arguments");
    CKIT_ASSERT(element != NULL, "fatal: ckit_deque_push invalid arguments");
    CKIT_ASSERT(deque->capacity > 0U, "fatal: ckit_deque_push invalid state");

    if (deque->size == deque->capacity) {
        ckit_deque_grow(deque);
    }

    uint8_t *base = (uint8_t *)deque->buffer;
    void *dst = base + (deque->tail * deque->elem_size);
    memcpy(dst, element, deque->elem_size);
    deque->size += 1;
    deque->tail = (deque->tail + 1) % deque->capacity;
}

void ckit_deque_pushfront(ckit_deque *deque, const void *element) {
    CKIT_ASSERT(deque != NULL, "fatal: ckit_deque_pushfront invalid arguments");
    CKIT_ASSERT(element != NULL, "fatal: ckit_deque_pushfront invalid arguments");
    CKIT_ASSERT(deque->capacity > 0U, "fatal: ckit_deque_pushfront invalid state");

    if (deque->size == deque->capacity) {
        ckit_deque_grow(deque);
    }

    deque->head = (deque->head + deque->capacity - 1) % deque->capacity;

    uint8_t *base = (uint8_t *)deque->buffer;
    void *dst = base + (deque->head * deque->elem_size);
    memcpy(dst, element, deque->elem_size);
    deque->size += 1;
}

void *ckit_deque_popleft(ckit_deque *deque) {
    CKIT_ASSERT(deque != NULL, "fatal: ckit_deque_popleft invalid arguments");
    CKIT_ASSERT(deque->capacity > 0U, "fatal: ckit_deque_popleft invalid state");

    if (deque->size == 0) {
        return NULL;
    }

    uint8_t *base = (uint8_t *)deque->buffer;
    void *src = base + (deque->head * deque->elem_size);

    deque->head = (deque->head + 1) % deque->capacity;
    deque->size -= 1;
    return src;
}

void *ckit_deque_popback(ckit_deque *deque) {
    CKIT_ASSERT(deque != NULL, "fatal: ckit_deque_popback invalid arguments");
    CKIT_ASSERT(deque->capacity > 0U, "fatal: ckit_deque_popback invalid state");

    if (deque->size == 0) {
        return NULL;
    }

    deque->tail = (deque->tail + deque->capacity - 1) % deque->capacity;

    uint8_t *base = (uint8_t *)deque->buffer;
    void *src = base + (deque->tail * deque->elem_size);

    deque->size -= 1;
    return src;
}

const void *ckit_deque_peekleft(const ckit_deque *deque) {
    CKIT_ASSERT(deque != NULL, "fatal: ckit_deque_peekleft invalid arguments");

    if (deque->size == 0) {
        return NULL;
    }

    uint8_t *base = (uint8_t *)deque->buffer;
    return base + (deque->head * deque->elem_size);
}

const void *ckit_deque_peekback(const ckit_deque *deque) {
    CKIT_ASSERT(deque != NULL, "fatal: ckit_deque_peekback invalid arguments");
    CKIT_ASSERT(deque->capacity > 0U, "fatal: ckit_deque_peekback invalid state");

    if (deque->size == 0) {
        return NULL;
    }

    size_t last_index = (deque->tail + deque->capacity - 1) % deque->capacity;
    uint8_t *base = (uint8_t *)deque->buffer;
    return base + (last_index * deque->elem_size);
}

void ckit_deque_free(ckit_deque *deque) {
    CKIT_ASSERT(deque != NULL, "fatal: ckit_deque_free invalid arguments");

    ckit_allocator *allocator = deque->allocator;
    ckit_dealloc(deque->allocator, deque->buffer);
    ckit_dealloc(allocator, deque);
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
