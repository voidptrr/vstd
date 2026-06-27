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
#include "vstd/ds/iterator.h"
#include "vstd/ds/vector.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/utils.h"

#define VECTOR_DEFAULT_CAPACITY 16

struct vector {
    size_t size;
    size_t elem_size;
    size_t capacity;
    void *buffer;
    allocator *allocator;
};

typedef struct vector_iterator_state {
    const uint8_t *cursor;
    size_t elem_size;
    size_t remaining;
} vector_iterator_state;

_Static_assert(
    sizeof(vector_iterator_state) <= ITERATOR_STATE_SIZE,
    "vector_iterator_state must fit in iterator"
);

static status vector_capacity_grow(size_t capacity, size_t min_capacity, size_t *out) {
    ASSERT(out != NULL, "fatal: vector_capacity_grow invalid arguments");

    if (capacity >= min_capacity) {
        *out = capacity;
        return STATUS_OK;
    }

    while (capacity < min_capacity) {
        size_t increment = capacity >> 1;
        if (increment == 0) {
            increment = 1;
        }

        size_t next = 0;
        if (size_add_overflow(capacity, increment, &next)) {
            capacity = min_capacity;
            break;
        }

        capacity = next;
    }

    *out = capacity;
    return STATUS_OK;
}

static const void *vector_iterator_next(void *context) {
    ASSERT(context != NULL, "fatal: vector_iterator_next invalid arguments");

    vector_iterator_state *iterator = context;
    if (iterator->remaining == 0) {
        return NULL;
    }

    const void *item = iterator->cursor;
    iterator->cursor += iterator->elem_size;
    iterator->remaining -= 1;
    return item;
}

status vector_create(size_t elem_size, allocator *allocator, vector **out) {
    return vector_create_with_capacity(elem_size, VECTOR_DEFAULT_CAPACITY, allocator, out);
}

status vector_create_with_capacity(
    size_t elem_size,
    size_t capacity,
    allocator *allocator,
    vector **out
) {
    ASSERT(elem_size > 0, "fatal: vector_create invalid arguments");
    ASSERT(capacity > 0, "fatal: vector_create invalid arguments");
    ASSERT(out != NULL, "fatal: vector_create invalid arguments");

    *out = NULL;

    vector *vector = NULL;
    status st = alloc(allocator, sizeof(*vector), (void **)&vector);
    if (st != STATUS_OK) {
        return st;
    }
    vector->allocator = allocator;

    size_t alloc_size = 0;
    if (size_mul_overflow(elem_size, capacity, &alloc_size)) {
        dealloc(allocator, vector);
        return STATUS_OVERFLOW;
    }

    void *buffer = NULL;
    st = alloc(allocator, alloc_size, &buffer);
    if (st != STATUS_OK) {
        dealloc(allocator, vector);
        return st;
    }

    vector->buffer = buffer;
    vector->size = 0;
    vector->elem_size = elem_size;
    vector->capacity = capacity;

    *out = vector;
    return STATUS_OK;
}

status vector_reserve(vector *vector, size_t capacity) {
    ASSERT(vector != NULL, "fatal: vector_reserve invalid arguments");

    if (capacity <= vector->capacity) {
        return STATUS_OK;
    }

    size_t alloc_size = 0;
    if (size_mul_overflow(capacity, vector->elem_size, &alloc_size)) {
        return STATUS_OVERFLOW;
    }

    void *buffer = NULL;
    status st = resize(vector->allocator, vector->buffer, alloc_size, &buffer);
    if (st != STATUS_OK) {
        return st;
    }

    vector->buffer = buffer;
    vector->capacity = capacity;
    return STATUS_OK;
}

status vector_push(vector *vector, const void *element) {
    ASSERT(vector != NULL, "fatal: vector_push invalid arguments");
    ASSERT(element != NULL, "fatal: vector_push invalid arguments");

    if (vector->size == vector->capacity) {
        size_t min_capacity = 0;
        if (size_add_overflow(vector->size, 1, &min_capacity)) {
            return STATUS_OVERFLOW;
        }

        size_t new_capacity = 0;
        RETURN_IF_ERROR(vector_capacity_grow(vector->capacity, min_capacity, &new_capacity));

        status st = vector_reserve(vector, new_capacity);
        if (st != STATUS_OK) {
            return st;
        }
    }

    uint8_t *base = (uint8_t *)vector->buffer;
    void *dst = base + (vector->size * vector->elem_size);
    memcpy(dst, element, vector->elem_size);
    vector->size += 1;
    return STATUS_OK;
}

void *vector_pop(vector *vector) {
    ASSERT(vector != NULL, "fatal: vector_pop invalid arguments");

    if (vector->size == 0) {
        return NULL;
    }

    vector->size -= 1;

    uint8_t *base = (uint8_t *)vector->buffer;
    return base + (vector->size * vector->elem_size);
}

void *vector_get(vector *vector, size_t index) {
    if (vector == NULL || index >= vector->size) {
        return NULL;
    }

    uint8_t *base = (uint8_t *)vector->buffer;
    return base + (index * vector->elem_size);
}

const void *vector_get_const(const vector *vector, size_t index) {
    if (vector == NULL || index >= vector->size) {
        return NULL;
    }

    const uint8_t *base = (const uint8_t *)vector->buffer;
    return base + (index * vector->elem_size);
}

size_t vector_elem_size(const vector *vector) {
    ASSERT(vector != NULL, "fatal: vector_elem_size invalid arguments");

    return vector->elem_size;
}

size_t vector_capacity(const vector *vector) {
    ASSERT(vector != NULL, "fatal: vector_capacity invalid arguments");

    return vector->capacity;
}

void *vector_data(vector *vector) {
    ASSERT(vector != NULL, "fatal: vector_data invalid arguments");

    return vector->buffer;
}

const void *vector_data_const(const vector *vector) {
    ASSERT(vector != NULL, "fatal: vector_data_const invalid arguments");

    return vector->buffer;
}

void *vector_swap_remove(vector *vector, size_t index) {
    ASSERT(vector != NULL, "fatal: vector_swap_remove invalid arguments");

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

size_t vector_size(const vector *vector) {
    ASSERT(vector != NULL, "fatal: vector_size invalid arguments");

    return vector->size;
}

iterator vector_get_iterator(const vector *vector) {
    ASSERT(vector != NULL, "fatal: vector_get_iterator invalid arguments");

    iterator iter = iterator_from_state(vector_iterator_next);
    vector_iterator_state *state = iterator_state(&iter);
    state->cursor = (const uint8_t *)vector->buffer;
    state->elem_size = vector->elem_size;
    state->remaining = vector->size;
    iterator_set_size_hint(&iter, vector->size);
    return iter;
}

size_t vector_lower_bound(const vector *vector, const void *key, vector_cmp_fn cmp) {
    ASSERT(vector != NULL, "fatal: vector_lower_bound invalid arguments");
    ASSERT(key != NULL, "fatal: vector_lower_bound invalid arguments");
    ASSERT(cmp != NULL, "fatal: vector_lower_bound invalid arguments");

    size_t first = 0;
    size_t count = vector->size;

    while (count > 0) {
        size_t step = count / 2;
        size_t mid = first + step;
        const void *item = vector_get_const(vector, mid);

        if (cmp(item, key) < 0) {
            first = mid + 1;
            count -= step + 1;
        } else {
            count = step;
        }
    }

    return first;
}

size_t vector_upper_bound(const vector *vector, const void *key, vector_cmp_fn cmp) {
    ASSERT(vector != NULL, "fatal: vector_upper_bound invalid arguments");
    ASSERT(key != NULL, "fatal: vector_upper_bound invalid arguments");
    ASSERT(cmp != NULL, "fatal: vector_upper_bound invalid arguments");

    size_t first = 0;
    size_t count = vector->size;

    while (count > 0) {
        size_t step = count / 2;
        size_t mid = first + step;
        const void *item = vector_get_const(vector, mid);

        if (cmp(key, item) >= 0) {
            first = mid + 1;
            count -= step + 1;
        } else {
            count = step;
        }
    }

    return first;
}

size_t vector_binary_search(const vector *vector, const void *key, vector_cmp_fn cmp) {
    size_t index = vector_lower_bound(vector, key, cmp);

    if (index < vector_size(vector) && cmp(vector_get_const(vector, index), key) == 0) {
        return index;
    }

    return vector_size(vector);
}

void vector_destroy(vector *vector) {
    ASSERT(vector != NULL, "fatal: vector_destroy invalid arguments");

    allocator *allocator = vector->allocator;
    dealloc(allocator, vector->buffer);
    dealloc(allocator, vector);
}
