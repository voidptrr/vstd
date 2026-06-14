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

#include "ckit/common/panic.h"
#include "ckit/datastruct/deque.h"
#include "ckit/memory/allocators/allocator.h"

#define CK_DEQUE_DEFAULT_CAPACITY 16

struct ck_deque {
    size_t size;
    size_t elem_size;
    size_t capacity;
    size_t head;
    size_t tail;
    void *buffer;
    ck_allocator *allocator;
};

static void ck_deque_grow(ck_deque *deque) {
    size_t old_capacity = deque->capacity;
    size_t new_capacity = old_capacity * 2;
    uint8_t *old_buffer = (uint8_t *)deque->buffer;

    size_t alloc_size = new_capacity * deque->elem_size;
    uint8_t *new_buffer = ck_malloc(deque->allocator, alloc_size);

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

    ck_dealloc(deque->allocator, deque->buffer);
    deque->buffer = new_buffer;
    deque->capacity = new_capacity;
    deque->head = 0;
    deque->tail = deque->size;
}

ck_deque *ck_deque_create(size_t elem_size, ck_allocator *allocator) {
    CK_ASSERT(elem_size > 0, "fatal: ck_deque_create invalid arguments");

    ck_deque *deque = ck_malloc(allocator, sizeof(ck_deque));
    deque->allocator = allocator;

    size_t alloc_size = elem_size * CK_DEQUE_DEFAULT_CAPACITY;
    void *buffer = ck_malloc(allocator, alloc_size);

    deque->size = 0;
    deque->elem_size = elem_size;
    deque->capacity = CK_DEQUE_DEFAULT_CAPACITY;
    deque->head = 0;
    deque->tail = 0;
    deque->buffer = buffer;

    return deque;
}

void ck_deque_push(ck_deque *deque, const void *element) {
    CK_ASSERT(deque != NULL, "fatal: ck_deque_push invalid arguments");
    CK_ASSERT(element != NULL, "fatal: ck_deque_push invalid arguments");
    CK_ASSERT(deque->capacity > 0, "fatal: ck_deque_push invalid state");

    if (deque->size == deque->capacity) {
        ck_deque_grow(deque);
    }

    uint8_t *base = (uint8_t *)deque->buffer;
    void *dst = base + (deque->tail * deque->elem_size);
    memcpy(dst, element, deque->elem_size);
    deque->size += 1;
    deque->tail = (deque->tail + 1) % deque->capacity;
}

void ck_deque_pushfront(ck_deque *deque, const void *element) {
    CK_ASSERT(deque != NULL, "fatal: ck_deque_pushfront invalid arguments");
    CK_ASSERT(element != NULL, "fatal: ck_deque_pushfront invalid arguments");
    CK_ASSERT(deque->capacity > 0, "fatal: ck_deque_pushfront invalid state");

    if (deque->size == deque->capacity) {
        ck_deque_grow(deque);
    }

    deque->head = (deque->head + deque->capacity - 1) % deque->capacity;

    uint8_t *base = (uint8_t *)deque->buffer;
    void *dst = base + (deque->head * deque->elem_size);
    memcpy(dst, element, deque->elem_size);
    deque->size += 1;
}

void *ck_deque_popleft(ck_deque *deque) {
    CK_ASSERT(deque != NULL, "fatal: ck_deque_popleft invalid arguments");
    CK_ASSERT(deque->capacity > 0, "fatal: ck_deque_popleft invalid state");

    if (deque->size == 0) {
        return NULL;
    }

    uint8_t *base = (uint8_t *)deque->buffer;
    void *src = base + (deque->head * deque->elem_size);

    deque->head = (deque->head + 1) % deque->capacity;
    deque->size -= 1;
    return src;
}

void *ck_deque_popback(ck_deque *deque) {
    CK_ASSERT(deque != NULL, "fatal: ck_deque_popback invalid arguments");
    CK_ASSERT(deque->capacity > 0, "fatal: ck_deque_popback invalid state");

    if (deque->size == 0) {
        return NULL;
    }

    deque->tail = (deque->tail + deque->capacity - 1) % deque->capacity;

    uint8_t *base = (uint8_t *)deque->buffer;
    void *src = base + (deque->tail * deque->elem_size);

    deque->size -= 1;
    return src;
}

const void *ck_deque_peekleft(const ck_deque *deque) {
    CK_ASSERT(deque != NULL, "fatal: ck_deque_peekleft invalid arguments");

    if (deque->size == 0) {
        return NULL;
    }

    uint8_t *base = (uint8_t *)deque->buffer;
    return base + (deque->head * deque->elem_size);
}

const void *ck_deque_peekback(const ck_deque *deque) {
    CK_ASSERT(deque != NULL, "fatal: ck_deque_peekback invalid arguments");
    CK_ASSERT(deque->capacity > 0, "fatal: ck_deque_peekback invalid state");

    if (deque->size == 0) {
        return NULL;
    }

    size_t last_index = (deque->tail + deque->capacity - 1) % deque->capacity;
    uint8_t *base = (uint8_t *)deque->buffer;
    return base + (last_index * deque->elem_size);
}

size_t ck_deque_size(const ck_deque *deque) {
    CK_ASSERT(deque != NULL, "fatal: ck_deque_size invalid arguments");

    return deque->size;
}

void ck_deque_destroy(ck_deque *deque) {
    CK_ASSERT(deque != NULL, "fatal: ck_deque_destroy invalid arguments");

    ck_allocator *allocator = deque->allocator;
    ck_dealloc(deque->allocator, deque->buffer);
    ck_dealloc(allocator, deque);
}
