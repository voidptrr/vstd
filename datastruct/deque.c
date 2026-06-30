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

#include "allocators/size.h"
#include "k4c/allocators/allocator.h"
#include "k4c/assert.h"
#include "k4c/datastruct/deque.h"
#include "k4c/datastruct/iterator.h"
#include "k4c/error.h"

#define K4C_DEQUE_DEFAULT_CAPACITY 16

struct k4c_deque {
    size_t size;
    size_t elem_size;
    size_t capacity;
    size_t head;
    size_t tail;
    void *buffer;
    k4c_allocator k4c_allocator;
};

typedef struct k4c_deque_iterator_state {
    const uint8_t *base;
    size_t elem_size;
    size_t index;
    size_t mask;
    size_t remaining;
} k4c_deque_iterator_state;

_Static_assert(
    sizeof(k4c_deque_iterator_state) <= K4C_ITERATOR_STATE_SIZE,
    "k4c_deque_iterator_state must fit in k4c_iterator"
);

static k4c_status k4c_deque_capacity_grow(size_t capacity, size_t min_capacity, size_t *out) {
    K4C_ASSERT(out != NULL, "fatal: k4c_deque_capacity_grow invalid arguments");
    K4C_ASSERT(
        (capacity & (capacity - 1)) == 0,
        "fatal: k4c_deque_capacity_grow invalid arguments"
    );

    if (capacity >= min_capacity) {
        *out = capacity;
        return K4C_STATUS_OK;
    }

    while (capacity < min_capacity) {
        if (capacity > (SIZE_MAX >> 1)) {
            return K4C_STATUS_OVERFLOW;
        }

        capacity <<= 1;
    }

    *out = capacity;
    return K4C_STATUS_OK;
}

static k4c_status k4c_deque_grow(k4c_deque *k4c_deque) {
    k4c_allocator *k4c_allocator = &k4c_deque->k4c_allocator;
    size_t old_capacity = k4c_deque->capacity;
    size_t min_capacity = 0;
    if (k4c_size_add_overflow(k4c_deque->size, 1, &min_capacity)) {
        return K4C_STATUS_OVERFLOW;
    }

    size_t new_capacity = 0;
    K4C_RETURN_IF_ERROR(k4c_deque_capacity_grow(old_capacity, min_capacity, &new_capacity));

    uint8_t *old_buffer = (uint8_t *)k4c_deque->buffer;

    size_t alloc_size = 0;
    if (k4c_size_mul_overflow(new_capacity, k4c_deque->elem_size, &alloc_size)) {
        return K4C_STATUS_OVERFLOW;
    }

    uint8_t *new_buffer = NULL;
    k4c_status st = k4c_alloc(k4c_allocator, alloc_size, (void **)&new_buffer);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    if (k4c_deque->size > 0) {
        if (k4c_deque->head < k4c_deque->tail) {
            memcpy(
                new_buffer,
                old_buffer + (k4c_deque->head * k4c_deque->elem_size),
                k4c_deque->size * k4c_deque->elem_size
            );
        } else {
            size_t first_count = old_capacity - k4c_deque->head;
            size_t second_count = k4c_deque->tail;

            memcpy(
                new_buffer,
                old_buffer + (k4c_deque->head * k4c_deque->elem_size),
                first_count * k4c_deque->elem_size
            );
            if (second_count > 0) {
                memcpy(
                    new_buffer + (first_count * k4c_deque->elem_size),
                    old_buffer,
                    second_count * k4c_deque->elem_size
                );
            }
        }
    }

    k4c_dealloc(k4c_allocator, k4c_deque->buffer);
    k4c_deque->buffer = new_buffer;
    k4c_deque->capacity = new_capacity;
    k4c_deque->head = 0;
    k4c_deque->tail = k4c_deque->size;
    return K4C_STATUS_OK;
}

static const void *k4c_deque_iterator_next(void *context) {
    K4C_ASSERT(context != NULL, "fatal: k4c_deque_iterator_next invalid arguments");

    k4c_deque_iterator_state *k4c_iterator = context;
    if (k4c_iterator->remaining == 0) {
        return NULL;
    }

    size_t storage_index = k4c_iterator->index & k4c_iterator->mask;
    k4c_iterator->index += 1;
    k4c_iterator->remaining -= 1;
    return k4c_iterator->base + (storage_index * k4c_iterator->elem_size);
}

k4c_status k4c_deque_create(size_t elem_size, k4c_allocator *k4c_allocator, k4c_deque **out) {
    K4C_ASSERT(elem_size > 0, "fatal: k4c_deque_create invalid arguments");
    K4C_ASSERT(out != NULL, "fatal: k4c_deque_create invalid arguments");

    *out = NULL;

    size_t alloc_size = 0;
    if (k4c_size_mul_overflow(elem_size, K4C_DEQUE_DEFAULT_CAPACITY, &alloc_size)) {
        return K4C_STATUS_OVERFLOW;
    }

    k4c_deque *k4c_deque = NULL;
    k4c_status st = k4c_alloc(k4c_allocator, sizeof(*k4c_deque), (void **)&k4c_deque);
    if (st != K4C_STATUS_OK) {
        return st;
    }
    k4c_deque->k4c_allocator = k4c_allocator_copy(k4c_allocator);

    void *buffer = NULL;
    st = k4c_alloc(&k4c_deque->k4c_allocator, alloc_size, &buffer);
    if (st != K4C_STATUS_OK) {
        k4c_dealloc(&k4c_deque->k4c_allocator, k4c_deque);
        return st;
    }

    k4c_deque->size = 0;
    k4c_deque->elem_size = elem_size;
    k4c_deque->capacity = K4C_DEQUE_DEFAULT_CAPACITY;
    k4c_deque->head = 0;
    k4c_deque->tail = 0;
    k4c_deque->buffer = buffer;

    *out = k4c_deque;
    return K4C_STATUS_OK;
}

k4c_status k4c_deque_push(k4c_deque *k4c_deque, const void *element) {
    K4C_ASSERT(k4c_deque != NULL, "fatal: k4c_deque_push invalid arguments");
    K4C_ASSERT(element != NULL, "fatal: k4c_deque_push invalid arguments");
    K4C_ASSERT(k4c_deque->capacity > 0, "fatal: k4c_deque_push invalid state");

    if (k4c_deque->size == k4c_deque->capacity) {
        k4c_status st = k4c_deque_grow(k4c_deque);
        if (st != K4C_STATUS_OK) {
            return st;
        }
    }

    uint8_t *base = (uint8_t *)k4c_deque->buffer;
    void *dst = base + (k4c_deque->tail * k4c_deque->elem_size);
    memcpy(dst, element, k4c_deque->elem_size);
    k4c_deque->size += 1;
    k4c_deque->tail = (k4c_deque->tail + 1) % k4c_deque->capacity;
    return K4C_STATUS_OK;
}

k4c_status k4c_deque_pushfront(k4c_deque *k4c_deque, const void *element) {
    K4C_ASSERT(k4c_deque != NULL, "fatal: k4c_deque_pushfront invalid arguments");
    K4C_ASSERT(element != NULL, "fatal: k4c_deque_pushfront invalid arguments");
    K4C_ASSERT(k4c_deque->capacity > 0, "fatal: k4c_deque_pushfront invalid state");

    if (k4c_deque->size == k4c_deque->capacity) {
        k4c_status st = k4c_deque_grow(k4c_deque);
        if (st != K4C_STATUS_OK) {
            return st;
        }
    }

    k4c_deque->head = (k4c_deque->head + k4c_deque->capacity - 1) % k4c_deque->capacity;

    uint8_t *base = (uint8_t *)k4c_deque->buffer;
    void *dst = base + (k4c_deque->head * k4c_deque->elem_size);
    memcpy(dst, element, k4c_deque->elem_size);
    k4c_deque->size += 1;
    return K4C_STATUS_OK;
}

void *k4c_deque_popleft(k4c_deque *k4c_deque) {
    K4C_ASSERT(k4c_deque != NULL, "fatal: k4c_deque_popleft invalid arguments");
    K4C_ASSERT(k4c_deque->capacity > 0, "fatal: k4c_deque_popleft invalid state");

    if (k4c_deque->size == 0) {
        return NULL;
    }

    uint8_t *base = (uint8_t *)k4c_deque->buffer;
    void *src = base + (k4c_deque->head * k4c_deque->elem_size);

    k4c_deque->head = (k4c_deque->head + 1) % k4c_deque->capacity;
    k4c_deque->size -= 1;
    return src;
}

void *k4c_deque_popback(k4c_deque *k4c_deque) {
    K4C_ASSERT(k4c_deque != NULL, "fatal: k4c_deque_popback invalid arguments");
    K4C_ASSERT(k4c_deque->capacity > 0, "fatal: k4c_deque_popback invalid state");

    if (k4c_deque->size == 0) {
        return NULL;
    }

    k4c_deque->tail = (k4c_deque->tail + k4c_deque->capacity - 1) % k4c_deque->capacity;

    uint8_t *base = (uint8_t *)k4c_deque->buffer;
    void *src = base + (k4c_deque->tail * k4c_deque->elem_size);

    k4c_deque->size -= 1;
    return src;
}

const void *k4c_deque_peekleft(const k4c_deque *k4c_deque) {
    K4C_ASSERT(k4c_deque != NULL, "fatal: k4c_deque_peekleft invalid arguments");

    if (k4c_deque->size == 0) {
        return NULL;
    }

    uint8_t *base = (uint8_t *)k4c_deque->buffer;
    return base + (k4c_deque->head * k4c_deque->elem_size);
}

const void *k4c_deque_peekback(const k4c_deque *k4c_deque) {
    K4C_ASSERT(k4c_deque != NULL, "fatal: k4c_deque_peekback invalid arguments");
    K4C_ASSERT(k4c_deque->capacity > 0, "fatal: k4c_deque_peekback invalid state");

    if (k4c_deque->size == 0) {
        return NULL;
    }

    size_t last_index = (k4c_deque->tail + k4c_deque->capacity - 1) % k4c_deque->capacity;
    uint8_t *base = (uint8_t *)k4c_deque->buffer;
    return base + (last_index * k4c_deque->elem_size);
}

size_t k4c_deque_size(const k4c_deque *k4c_deque) {
    K4C_ASSERT(k4c_deque != NULL, "fatal: k4c_deque_size invalid arguments");

    return k4c_deque->size;
}

k4c_iterator k4c_deque_get_iterator(const k4c_deque *k4c_deque) {
    K4C_ASSERT(k4c_deque != NULL, "fatal: k4c_deque_get_iterator invalid arguments");
    K4C_ASSERT(
        (k4c_deque->capacity & (k4c_deque->capacity - 1)) == 0,
        "fatal: k4c_deque invalid state"
    );

    k4c_iterator iter = k4c_iterator_from_state(k4c_deque_iterator_next);
    k4c_deque_iterator_state *state = k4c_iterator_state(&iter);
    state->base = (const uint8_t *)k4c_deque->buffer;
    state->elem_size = k4c_deque->elem_size;
    state->index = k4c_deque->head;
    state->mask = k4c_deque->capacity - 1;
    state->remaining = k4c_deque->size;
    k4c_iterator_set_size_hint(&iter, k4c_deque->size);
    return iter;
}

void k4c_deque_destroy(k4c_deque *k4c_deque) {
    K4C_ASSERT(k4c_deque != NULL, "fatal: k4c_deque_destroy invalid arguments");

    k4c_allocator *k4c_allocator = &k4c_deque->k4c_allocator;
    k4c_dealloc(k4c_allocator, k4c_deque->buffer);
    k4c_dealloc(k4c_allocator, k4c_deque);
}
