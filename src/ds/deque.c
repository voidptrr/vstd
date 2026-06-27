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
#include "vstd/ds/deque.h"
#include "vstd/ds/iterator.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/utils.h"

#define DEQUE_DEFAULT_CAPACITY 16

struct deque {
    size_t size;
    size_t elem_size;
    size_t capacity;
    size_t head;
    size_t tail;
    void *buffer;
    allocator *allocator;
};

typedef struct deque_iterator_state {
    const uint8_t *base;
    size_t elem_size;
    size_t index;
    size_t mask;
    size_t remaining;
} deque_iterator_state;

_Static_assert(
    sizeof(deque_iterator_state) <= ITERATOR_STATE_SIZE,
    "deque_iterator_state must fit in iterator"
);

static status deque_capacity_grow(size_t capacity, size_t min_capacity, size_t *out) {
    ASSERT(out != NULL, "fatal: deque_capacity_grow invalid arguments");
    ASSERT((capacity & (capacity - 1)) == 0, "fatal: deque_capacity_grow invalid arguments");

    if (capacity >= min_capacity) {
        *out = capacity;
        return STATUS_OK;
    }

    while (capacity < min_capacity) {
        if (capacity > (SIZE_MAX >> 1)) {
            return STATUS_OVERFLOW;
        }

        capacity <<= 1;
    }

    *out = capacity;
    return STATUS_OK;
}

static status deque_grow(deque *deque) {
    allocator *allocator = deque->allocator;
    size_t old_capacity = deque->capacity;
    size_t min_capacity = 0;
    if (size_add_overflow(deque->size, 1, &min_capacity)) {
        return STATUS_OVERFLOW;
    }

    size_t new_capacity = 0;
    RETURN_IF_ERROR(deque_capacity_grow(old_capacity, min_capacity, &new_capacity));

    uint8_t *old_buffer = (uint8_t *)deque->buffer;

    size_t alloc_size = 0;
    if (size_mul_overflow(new_capacity, deque->elem_size, &alloc_size)) {
        return STATUS_OVERFLOW;
    }

    uint8_t *new_buffer = NULL;
    status st = alloc(allocator, alloc_size, (void **)&new_buffer);
    if (st != STATUS_OK) {
        return st;
    }

    if (deque->size > 0) {
        if (deque->head < deque->tail) {
            memcpy(
                new_buffer,
                old_buffer + (deque->head * deque->elem_size),
                deque->size * deque->elem_size
            );
        } else {
            size_t first_count = old_capacity - deque->head;
            size_t second_count = deque->tail;

            memcpy(
                new_buffer,
                old_buffer + (deque->head * deque->elem_size),
                first_count * deque->elem_size
            );
            if (second_count > 0) {
                memcpy(
                    new_buffer + (first_count * deque->elem_size),
                    old_buffer,
                    second_count * deque->elem_size
                );
            }
        }
    }

    dealloc(allocator, deque->buffer);
    deque->buffer = new_buffer;
    deque->capacity = new_capacity;
    deque->head = 0;
    deque->tail = deque->size;
    return STATUS_OK;
}

static const void *deque_iterator_next(void *context) {
    ASSERT(context != NULL, "fatal: deque_iterator_next invalid arguments");

    deque_iterator_state *iterator = context;
    if (iterator->remaining == 0) {
        return NULL;
    }

    size_t storage_index = iterator->index & iterator->mask;
    iterator->index += 1;
    iterator->remaining -= 1;
    return iterator->base + (storage_index * iterator->elem_size);
}

status deque_create(size_t elem_size, allocator *allocator, deque **out) {
    ASSERT(elem_size > 0, "fatal: deque_create invalid arguments");
    ASSERT(out != NULL, "fatal: deque_create invalid arguments");

    *out = NULL;

    deque *deque = NULL;
    status st = alloc(allocator, sizeof(*deque), (void **)&deque);
    if (st != STATUS_OK) {
        return st;
    }
    deque->allocator = allocator;

    size_t alloc_size = 0;
    if (size_mul_overflow(elem_size, DEQUE_DEFAULT_CAPACITY, &alloc_size)) {
        dealloc(allocator, deque);
        return STATUS_OVERFLOW;
    }

    void *buffer = NULL;
    st = alloc(allocator, alloc_size, &buffer);
    if (st != STATUS_OK) {
        dealloc(allocator, deque);
        return st;
    }

    deque->size = 0;
    deque->elem_size = elem_size;
    deque->capacity = DEQUE_DEFAULT_CAPACITY;
    deque->head = 0;
    deque->tail = 0;
    deque->buffer = buffer;

    *out = deque;
    return STATUS_OK;
}

status deque_push(deque *deque, const void *element) {
    ASSERT(deque != NULL, "fatal: deque_push invalid arguments");
    ASSERT(element != NULL, "fatal: deque_push invalid arguments");
    ASSERT(deque->capacity > 0, "fatal: deque_push invalid state");

    if (deque->size == deque->capacity) {
        status st = deque_grow(deque);
        if (st != STATUS_OK) {
            return st;
        }
    }

    uint8_t *base = (uint8_t *)deque->buffer;
    void *dst = base + (deque->tail * deque->elem_size);
    memcpy(dst, element, deque->elem_size);
    deque->size += 1;
    deque->tail = (deque->tail + 1) % deque->capacity;
    return STATUS_OK;
}

status deque_pushfront(deque *deque, const void *element) {
    ASSERT(deque != NULL, "fatal: deque_pushfront invalid arguments");
    ASSERT(element != NULL, "fatal: deque_pushfront invalid arguments");
    ASSERT(deque->capacity > 0, "fatal: deque_pushfront invalid state");

    if (deque->size == deque->capacity) {
        status st = deque_grow(deque);
        if (st != STATUS_OK) {
            return st;
        }
    }

    deque->head = (deque->head + deque->capacity - 1) % deque->capacity;

    uint8_t *base = (uint8_t *)deque->buffer;
    void *dst = base + (deque->head * deque->elem_size);
    memcpy(dst, element, deque->elem_size);
    deque->size += 1;
    return STATUS_OK;
}

void *deque_popleft(deque *deque) {
    ASSERT(deque != NULL, "fatal: deque_popleft invalid arguments");
    ASSERT(deque->capacity > 0, "fatal: deque_popleft invalid state");

    if (deque->size == 0) {
        return NULL;
    }

    uint8_t *base = (uint8_t *)deque->buffer;
    void *src = base + (deque->head * deque->elem_size);

    deque->head = (deque->head + 1) % deque->capacity;
    deque->size -= 1;
    return src;
}

void *deque_popback(deque *deque) {
    ASSERT(deque != NULL, "fatal: deque_popback invalid arguments");
    ASSERT(deque->capacity > 0, "fatal: deque_popback invalid state");

    if (deque->size == 0) {
        return NULL;
    }

    deque->tail = (deque->tail + deque->capacity - 1) % deque->capacity;

    uint8_t *base = (uint8_t *)deque->buffer;
    void *src = base + (deque->tail * deque->elem_size);

    deque->size -= 1;
    return src;
}

const void *deque_peekleft(const deque *deque) {
    ASSERT(deque != NULL, "fatal: deque_peekleft invalid arguments");

    if (deque->size == 0) {
        return NULL;
    }

    uint8_t *base = (uint8_t *)deque->buffer;
    return base + (deque->head * deque->elem_size);
}

const void *deque_peekback(const deque *deque) {
    ASSERT(deque != NULL, "fatal: deque_peekback invalid arguments");
    ASSERT(deque->capacity > 0, "fatal: deque_peekback invalid state");

    if (deque->size == 0) {
        return NULL;
    }

    size_t last_index = (deque->tail + deque->capacity - 1) % deque->capacity;
    uint8_t *base = (uint8_t *)deque->buffer;
    return base + (last_index * deque->elem_size);
}

size_t deque_size(const deque *deque) {
    ASSERT(deque != NULL, "fatal: deque_size invalid arguments");

    return deque->size;
}

iterator deque_get_iterator(const deque *deque) {
    ASSERT(deque != NULL, "fatal: deque_get_iterator invalid arguments");
    ASSERT((deque->capacity & (deque->capacity - 1)) == 0, "fatal: deque invalid state");

    iterator iter = iterator_from_state(deque_iterator_next);
    deque_iterator_state *state = iterator_state(&iter);
    state->base = (const uint8_t *)deque->buffer;
    state->elem_size = deque->elem_size;
    state->index = deque->head;
    state->mask = deque->capacity - 1;
    state->remaining = deque->size;
    iterator_set_size_hint(&iter, deque->size);
    return iter;
}

void deque_destroy(deque *deque) {
    ASSERT(deque != NULL, "fatal: deque_destroy invalid arguments");

    allocator *allocator = deque->allocator;
    dealloc(allocator, deque->buffer);
    dealloc(allocator, deque);
}
