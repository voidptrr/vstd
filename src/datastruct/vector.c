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

#include "vstd/datastruct/vector.h"
#include "vstd/memory/allocators/allocator.h"
#include "vstd/panic.h"

#define VS_VECTOR_DEFAULT_CAPACITY 16

struct vs_vector {
    size_t size;
    size_t elem_size;
    size_t capacity;
    void *buffer;
    vs_allocator *allocator;
};

vs_vector *vs_vector_create(size_t elem_size, vs_allocator *allocator) {
    VS_ASSERT(elem_size > 0, "fatal: vs_vector_create invalid arguments");

    vs_vector *vector = vs_malloc(allocator, sizeof(vs_vector));
    vector->allocator = allocator;

    size_t alloc_size = elem_size * VS_VECTOR_DEFAULT_CAPACITY;
    void *buffer = vs_malloc(allocator, alloc_size);

    vector->buffer = buffer;
    vector->size = 0;
    vector->elem_size = elem_size;
    vector->capacity = VS_VECTOR_DEFAULT_CAPACITY;

    return vector;
}

void vs_vector_push(vs_vector *vector, const void *element) {
    VS_ASSERT(vector != NULL, "fatal: vs_vector_push invalid arguments");
    VS_ASSERT(element != NULL, "fatal: vs_vector_push invalid arguments");

    if (vector->size == vector->capacity) {
        vs_allocator *allocator = vector->allocator;
        size_t new_capacity = vector->capacity * 2;
        size_t alloc_size = new_capacity * vector->elem_size;

        void *tmp = vs_realloc(allocator, vector->buffer, alloc_size);
        vector->buffer = tmp;
        vector->capacity = new_capacity;
    }

    uint8_t *base = (uint8_t *)vector->buffer;
    void *dst = base + (vector->size * vector->elem_size);
    memcpy(dst, element, vector->elem_size);
    vector->size += 1;
}

void *vs_vector_pop(vs_vector *vector) {
    VS_ASSERT(vector != NULL, "fatal: vs_vector_pop invalid arguments");

    if (vector->size == 0) {
        return NULL;
    }

    vector->size -= 1;

    uint8_t *base = (uint8_t *)vector->buffer;
    return base + (vector->size * vector->elem_size);
}

void *vs_vector_get(vs_vector *vector, size_t index) {
    if (vector == NULL || index >= vector->size) {
        return NULL;
    }

    uint8_t *base = (uint8_t *)vector->buffer;
    return base + (index * vector->elem_size);
}

const void *vs_vector_get_const(const vs_vector *vector, size_t index) {
    if (vector == NULL || index >= vector->size) {
        return NULL;
    }

    const uint8_t *base = (const uint8_t *)vector->buffer;
    return base + (index * vector->elem_size);
}

size_t vs_vector_elem_size(const vs_vector *vector) {
    VS_ASSERT(vector != NULL, "fatal: vs_vector_elem_size invalid arguments");

    return vector->elem_size;
}

void *vs_vector_swap_remove(vs_vector *vector, size_t index) {
    VS_ASSERT(vector != NULL, "fatal: vs_vector_swap_remove invalid arguments");

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

size_t vs_vector_size(const vs_vector *vector) {
    VS_ASSERT(vector != NULL, "fatal: vs_vector_size invalid arguments");

    return vector->size;
}

void vs_vector_destroy(vs_vector *vector) {
    VS_ASSERT(vector != NULL, "fatal: vs_vector_destroy invalid arguments");

    vs_allocator *allocator = vector->allocator;
    vs_dealloc(allocator, vector->buffer);
    vs_dealloc(allocator, vector);
}
