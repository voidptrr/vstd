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
#include "vstd/datastruct/iterator.h"
#include "vstd/datastruct/vector.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/utils.h"

#define VS_VECTOR_DEFAULT_CAPACITY 16

struct vs_vector {
    size_t size;
    size_t elem_size;
    size_t capacity;
    void *buffer;
    vs_allocator *allocator;
};

typedef struct vs_vector_iterator_state {
    const uint8_t *cursor;
    size_t elem_size;
    size_t remaining;
} vs_vector_iterator_state;

_Static_assert(
    sizeof(vs_vector_iterator_state) <= VS_ITERATOR_STATE_SIZE,
    "vs_vector_iterator_state must fit in vs_iterator"
);

static vs_status vs_vector_capacity_grow(size_t capacity, size_t min_capacity, size_t *out) {
    VSTD_ASSERT(out != NULL, "fatal: vs_vector_capacity_grow invalid arguments");

    if (capacity >= min_capacity) {
        *out = capacity;
        return VS_STATUS_OK;
    }

    while (capacity < min_capacity) {
        size_t increment = capacity >> 1;
        if (increment == 0) {
            increment = 1;
        }

        size_t next = 0;
        if (vs_size_add_overflow(capacity, increment, &next)) {
            capacity = min_capacity;
            break;
        }

        capacity = next;
    }

    *out = capacity;
    return VS_STATUS_OK;
}

static const void *vs_vector_iterator_next(void *context) {
    VSTD_ASSERT(context != NULL, "fatal: vs_vector_iterator_next invalid arguments");

    vs_vector_iterator_state *iterator = context;
    if (iterator->remaining == 0) {
        return NULL;
    }

    const void *item = iterator->cursor;
    iterator->cursor += iterator->elem_size;
    iterator->remaining -= 1;
    return item;
}

vs_status vs_vector_create(size_t elem_size, vs_allocator *allocator, vs_vector **out) {
    return vs_vector_create_with_capacity(elem_size, VS_VECTOR_DEFAULT_CAPACITY, allocator, out);
}

vs_status vs_vector_create_with_capacity(
    size_t elem_size,
    size_t capacity,
    vs_allocator *allocator,
    vs_vector **out
) {
    VSTD_ASSERT(elem_size > 0, "fatal: vs_vector_create invalid arguments");
    VSTD_ASSERT(capacity > 0, "fatal: vs_vector_create invalid arguments");
    VSTD_ASSERT(out != NULL, "fatal: vs_vector_create invalid arguments");

    *out = NULL;

    vs_vector *vector = NULL;
    vs_status status = vs_alloc(allocator, sizeof(vs_vector), (void **)&vector);
    if (status != VS_STATUS_OK) {
        return status;
    }
    vector->allocator = allocator;

    size_t alloc_size = 0;
    if (vs_size_mul_overflow(elem_size, capacity, &alloc_size)) {
        vs_dealloc(allocator, vector);
        return VS_STATUS_OVERFLOW;
    }

    void *buffer = NULL;
    status = vs_alloc(allocator, alloc_size, &buffer);
    if (status != VS_STATUS_OK) {
        vs_dealloc(allocator, vector);
        return status;
    }

    vector->buffer = buffer;
    vector->size = 0;
    vector->elem_size = elem_size;
    vector->capacity = capacity;

    *out = vector;
    return VS_STATUS_OK;
}

vs_status vs_vector_reserve(vs_vector *vector, size_t capacity) {
    VSTD_ASSERT(vector != NULL, "fatal: vs_vector_reserve invalid arguments");

    if (capacity <= vector->capacity) {
        return VS_STATUS_OK;
    }

    size_t alloc_size = 0;
    if (vs_size_mul_overflow(capacity, vector->elem_size, &alloc_size)) {
        return VS_STATUS_OVERFLOW;
    }

    void *buffer = NULL;
    vs_status status = vs_resize(vector->allocator, vector->buffer, alloc_size, &buffer);
    if (status != VS_STATUS_OK) {
        return status;
    }

    vector->buffer = buffer;
    vector->capacity = capacity;
    return VS_STATUS_OK;
}

vs_status vs_vector_push(vs_vector *vector, const void *element) {
    VSTD_ASSERT(vector != NULL, "fatal: vs_vector_push invalid arguments");
    VSTD_ASSERT(element != NULL, "fatal: vs_vector_push invalid arguments");

    if (vector->size == vector->capacity) {
        size_t min_capacity = 0;
        if (vs_size_add_overflow(vector->size, 1, &min_capacity)) {
            return VS_STATUS_OVERFLOW;
        }

        size_t new_capacity = 0;
        VS_RETURN_IF_ERROR(vs_vector_capacity_grow(vector->capacity, min_capacity, &new_capacity));

        vs_status status = vs_vector_reserve(vector, new_capacity);
        if (status != VS_STATUS_OK) {
            return status;
        }
    }

    uint8_t *base = (uint8_t *)vector->buffer;
    void *dst = base + (vector->size * vector->elem_size);
    memcpy(dst, element, vector->elem_size);
    vector->size += 1;
    return VS_STATUS_OK;
}

void *vs_vector_pop(vs_vector *vector) {
    VSTD_ASSERT(vector != NULL, "fatal: vs_vector_pop invalid arguments");

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
    VSTD_ASSERT(vector != NULL, "fatal: vs_vector_elem_size invalid arguments");

    return vector->elem_size;
}

size_t vs_vector_capacity(const vs_vector *vector) {
    VSTD_ASSERT(vector != NULL, "fatal: vs_vector_capacity invalid arguments");

    return vector->capacity;
}

void *vs_vector_data(vs_vector *vector) {
    VSTD_ASSERT(vector != NULL, "fatal: vs_vector_data invalid arguments");

    return vector->buffer;
}

const void *vs_vector_data_const(const vs_vector *vector) {
    VSTD_ASSERT(vector != NULL, "fatal: vs_vector_data_const invalid arguments");

    return vector->buffer;
}

void *vs_vector_swap_remove(vs_vector *vector, size_t index) {
    VSTD_ASSERT(vector != NULL, "fatal: vs_vector_swap_remove invalid arguments");

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
    VSTD_ASSERT(vector != NULL, "fatal: vs_vector_size invalid arguments");

    return vector->size;
}

vs_iterator vs_vector_get_iterator(const vs_vector *vector) {
    VSTD_ASSERT(vector != NULL, "fatal: vs_vector_get_iterator invalid arguments");

    vs_iterator iter = vs_iterator_from_state(vs_vector_iterator_next);
    vs_vector_iterator_state *state = vs_iterator_state(&iter);
    state->cursor = (const uint8_t *)vector->buffer;
    state->elem_size = vector->elem_size;
    state->remaining = vector->size;
    vs_iterator_set_size_hint(&iter, vector->size);
    return iter;
}

size_t vs_vector_lower_bound(const vs_vector *vector, const void *key, vs_vector_cmp_fn cmp) {
    VSTD_ASSERT(vector != NULL, "fatal: vs_vector_lower_bound invalid arguments");
    VSTD_ASSERT(key != NULL, "fatal: vs_vector_lower_bound invalid arguments");
    VSTD_ASSERT(cmp != NULL, "fatal: vs_vector_lower_bound invalid arguments");

    size_t first = 0;
    size_t count = vector->size;

    while (count > 0) {
        size_t step = count / 2;
        size_t mid = first + step;
        const void *item = vs_vector_get_const(vector, mid);

        if (cmp(item, key) < 0) {
            first = mid + 1;
            count -= step + 1;
        } else {
            count = step;
        }
    }

    return first;
}

size_t vs_vector_upper_bound(const vs_vector *vector, const void *key, vs_vector_cmp_fn cmp) {
    VSTD_ASSERT(vector != NULL, "fatal: vs_vector_upper_bound invalid arguments");
    VSTD_ASSERT(key != NULL, "fatal: vs_vector_upper_bound invalid arguments");
    VSTD_ASSERT(cmp != NULL, "fatal: vs_vector_upper_bound invalid arguments");

    size_t first = 0;
    size_t count = vector->size;

    while (count > 0) {
        size_t step = count / 2;
        size_t mid = first + step;
        const void *item = vs_vector_get_const(vector, mid);

        if (cmp(key, item) >= 0) {
            first = mid + 1;
            count -= step + 1;
        } else {
            count = step;
        }
    }

    return first;
}

size_t vs_vector_binary_search(const vs_vector *vector, const void *key, vs_vector_cmp_fn cmp) {
    size_t index = vs_vector_lower_bound(vector, key, cmp);

    if (index < vs_vector_size(vector) && cmp(vs_vector_get_const(vector, index), key) == 0) {
        return index;
    }

    return vs_vector_size(vector);
}

void vs_vector_destroy(vs_vector *vector) {
    VSTD_ASSERT(vector != NULL, "fatal: vs_vector_destroy invalid arguments");

    vs_allocator *allocator = vector->allocator;
    vs_dealloc(allocator, vector->buffer);
    vs_dealloc(allocator, vector);
}
