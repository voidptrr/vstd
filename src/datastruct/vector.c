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
#include "ckit/datastruct/vector.h"
#include "ckit/memory/allocators/allocator.h"

#define CK_VECTOR_DEFAULT_CAPACITY 16

struct ck_vector {
    size_t size;
    size_t elem_size;
    size_t capacity;
    void *buffer;
    ck_allocator *allocator;
};

ck_vector *ck_vector_create(size_t elem_size, ck_allocator *allocator) {
    CK_ASSERT(elem_size > 0, "fatal: ck_vector_create invalid arguments");

    ck_vector *vector = ck_malloc(allocator, sizeof(ck_vector));
    vector->allocator = allocator;

    size_t alloc_size = elem_size * CK_VECTOR_DEFAULT_CAPACITY;
    void *buffer = ck_malloc(allocator, alloc_size);

    vector->buffer = buffer;
    vector->size = 0;
    vector->elem_size = elem_size;
    vector->capacity = CK_VECTOR_DEFAULT_CAPACITY;

    return vector;
}

void ck_vector_push(ck_vector *vector, const void *element) {
    CK_ASSERT(vector != NULL, "fatal: ck_vector_push invalid arguments");
    CK_ASSERT(element != NULL, "fatal: ck_vector_push invalid arguments");

    if (vector->size == vector->capacity) {
        size_t new_capacity = vector->capacity * 2;
        size_t alloc_size = new_capacity * vector->elem_size;

        void *tmp = ck_realloc(vector->allocator, vector->buffer, alloc_size);
        vector->buffer = tmp;
        vector->capacity = new_capacity;
    }

    uint8_t *base = (uint8_t *)vector->buffer;
    void *dst = base + (vector->size * vector->elem_size);
    memcpy(dst, element, vector->elem_size);
    vector->size += 1;
}

void *ck_vector_pop(ck_vector *vector) {
    CK_ASSERT(vector != NULL, "fatal: ck_vector_pop invalid arguments");

    if (vector->size == 0) {
        return NULL;
    }

    vector->size -= 1;

    uint8_t *base = (uint8_t *)vector->buffer;
    return base + (vector->size * vector->elem_size);
}

void *ck_vector_get(ck_vector *vector, size_t index) {
    if (vector == NULL || index >= vector->size) {
        return NULL;
    }

    uint8_t *base = (uint8_t *)vector->buffer;
    return base + (index * vector->elem_size);
}

const void *ck_vector_get_const(const ck_vector *vector, size_t index) {
    if (vector == NULL || index >= vector->size) {
        return NULL;
    }

    const uint8_t *base = (const uint8_t *)vector->buffer;
    return base + (index * vector->elem_size);
}

size_t ck_vector_elem_size(const ck_vector *vector) {
    CK_ASSERT(vector != NULL, "fatal: ck_vector_elem_size invalid arguments");

    return vector->elem_size;
}

void *ck_vector_swap_remove(ck_vector *vector, size_t index) {
    CK_ASSERT(vector != NULL, "fatal: ck_vector_swap_remove invalid arguments");

    if (index >= vector->size) {
        return NULL;
    }

    uint8_t *base = (uint8_t *)vector->buffer;
    void *slot = base + (index * vector->elem_size);
    size_t last = vector->size - 1;

    if (index != last) {
        void *last_slot = base + (last * vector->elem_size);
        memcpy(slot, last_slot, vector->elem_size);
    }

    vector->size = last;
    return slot;
}

size_t ck_vector_size(const ck_vector *vector) {
    CK_ASSERT(vector != NULL, "fatal: ck_vector_size invalid arguments");

    return vector->size;
}

void ck_vector_destroy(ck_vector *vector) {
    CK_ASSERT(vector != NULL, "fatal: ck_vector_destroy invalid arguments");

    ck_allocator *allocator = vector->allocator;
    ck_dealloc(vector->allocator, vector->buffer);
    ck_dealloc(allocator, vector);
}
