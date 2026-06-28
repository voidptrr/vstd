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

#include "k4c/assert.h"
#include "k4c/ds/iterator.h"
#include "k4c/ds/vector.h"
#include "k4c/error.h"
#include "k4c/memory/allocator.h"
#include "memory/size.h"

#define K4C_VECTOR_DEFAULT_CAPACITY 16

struct k4c_vector {
    size_t size;
    size_t elem_size;
    size_t capacity;
    void *buffer;
    k4c_allocator *k4c_allocator;
};

typedef struct k4c_vector_iterator_state {
    const uint8_t *cursor;
    size_t elem_size;
    size_t remaining;
} k4c_vector_iterator_state;

_Static_assert(
    sizeof(k4c_vector_iterator_state) <= K4C_ITERATOR_STATE_SIZE,
    "k4c_vector_iterator_state must fit in k4c_iterator"
);

static k4c_status k4c_vector_capacity_grow(size_t capacity, size_t min_capacity, size_t *out) {
    K4C_ASSERT(out != NULL, "fatal: k4c_vector_capacity_grow invalid arguments");

    if (capacity >= min_capacity) {
        *out = capacity;
        return K4C_STATUS_OK;
    }

    while (capacity < min_capacity) {
        size_t increment = capacity >> 1;
        if (increment == 0) {
            increment = 1;
        }

        size_t next = 0;
        if (k4c_size_add_overflow(capacity, increment, &next)) {
            capacity = min_capacity;
            break;
        }

        capacity = next;
    }

    *out = capacity;
    return K4C_STATUS_OK;
}

static const void *k4c_vector_iterator_next(void *context) {
    K4C_ASSERT(context != NULL, "fatal: k4c_vector_iterator_next invalid arguments");

    k4c_vector_iterator_state *k4c_iterator = context;
    if (k4c_iterator->remaining == 0) {
        return NULL;
    }

    const void *item = k4c_iterator->cursor;
    k4c_iterator->cursor += k4c_iterator->elem_size;
    k4c_iterator->remaining -= 1;
    return item;
}

k4c_status k4c_vector_create(size_t elem_size, k4c_allocator *k4c_allocator, k4c_vector **out) {
    return k4c_vector_create_with_capacity(
        elem_size,
        K4C_VECTOR_DEFAULT_CAPACITY,
        k4c_allocator,
        out
    );
}

k4c_status k4c_vector_create_with_capacity(
    size_t elem_size,
    size_t capacity,
    k4c_allocator *k4c_allocator,
    k4c_vector **out
) {
    K4C_ASSERT(elem_size > 0, "fatal: k4c_vector_create invalid arguments");
    K4C_ASSERT(capacity > 0, "fatal: k4c_vector_create invalid arguments");
    K4C_ASSERT(out != NULL, "fatal: k4c_vector_create invalid arguments");

    *out = NULL;

    k4c_vector *k4c_vector = NULL;
    k4c_status st = k4c_alloc(k4c_allocator, sizeof(*k4c_vector), (void **)&k4c_vector);
    if (st != K4C_STATUS_OK) {
        return st;
    }
    k4c_vector->k4c_allocator = k4c_allocator;

    size_t alloc_size = 0;
    if (k4c_size_mul_overflow(elem_size, capacity, &alloc_size)) {
        k4c_dealloc(k4c_allocator, k4c_vector);
        return K4C_STATUS_OVERFLOW;
    }

    void *buffer = NULL;
    st = k4c_alloc(k4c_allocator, alloc_size, &buffer);
    if (st != K4C_STATUS_OK) {
        k4c_dealloc(k4c_allocator, k4c_vector);
        return st;
    }

    k4c_vector->buffer = buffer;
    k4c_vector->size = 0;
    k4c_vector->elem_size = elem_size;
    k4c_vector->capacity = capacity;

    *out = k4c_vector;
    return K4C_STATUS_OK;
}

k4c_status k4c_vector_reserve(k4c_vector *k4c_vector, size_t capacity) {
    K4C_ASSERT(k4c_vector != NULL, "fatal: k4c_vector_reserve invalid arguments");

    if (capacity <= k4c_vector->capacity) {
        return K4C_STATUS_OK;
    }

    size_t alloc_size = 0;
    if (k4c_size_mul_overflow(capacity, k4c_vector->elem_size, &alloc_size)) {
        return K4C_STATUS_OVERFLOW;
    }

    void *buffer = NULL;
    k4c_status st = k4c_resize(k4c_vector->k4c_allocator, k4c_vector->buffer, alloc_size, &buffer);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    k4c_vector->buffer = buffer;
    k4c_vector->capacity = capacity;
    return K4C_STATUS_OK;
}

k4c_status k4c_vector_push(k4c_vector *k4c_vector, const void *element) {
    K4C_ASSERT(k4c_vector != NULL, "fatal: k4c_vector_push invalid arguments");
    K4C_ASSERT(element != NULL, "fatal: k4c_vector_push invalid arguments");

    if (k4c_vector->size == k4c_vector->capacity) {
        size_t min_capacity = 0;
        if (k4c_size_add_overflow(k4c_vector->size, 1, &min_capacity)) {
            return K4C_STATUS_OVERFLOW;
        }

        size_t new_capacity = 0;
        K4C_RETURN_IF_ERROR(
            k4c_vector_capacity_grow(k4c_vector->capacity, min_capacity, &new_capacity)
        );

        k4c_status st = k4c_vector_reserve(k4c_vector, new_capacity);
        if (st != K4C_STATUS_OK) {
            return st;
        }
    }

    uint8_t *base = (uint8_t *)k4c_vector->buffer;
    void *dst = base + (k4c_vector->size * k4c_vector->elem_size);
    memcpy(dst, element, k4c_vector->elem_size);
    k4c_vector->size += 1;
    return K4C_STATUS_OK;
}

void *k4c_vector_pop(k4c_vector *k4c_vector) {
    K4C_ASSERT(k4c_vector != NULL, "fatal: k4c_vector_pop invalid arguments");

    if (k4c_vector->size == 0) {
        return NULL;
    }

    k4c_vector->size -= 1;

    uint8_t *base = (uint8_t *)k4c_vector->buffer;
    return base + (k4c_vector->size * k4c_vector->elem_size);
}

void *k4c_vector_get(const k4c_vector *k4c_vector, size_t index) {
    if (k4c_vector == NULL || index >= k4c_vector->size) {
        return NULL;
    }

    uint8_t *base = (uint8_t *)k4c_vector->buffer;
    return base + (index * k4c_vector->elem_size);
}

size_t k4c_vector_elem_size(const k4c_vector *k4c_vector) {
    K4C_ASSERT(k4c_vector != NULL, "fatal: k4c_vector_elem_size invalid arguments");

    return k4c_vector->elem_size;
}

size_t k4c_vector_capacity(const k4c_vector *k4c_vector) {
    K4C_ASSERT(k4c_vector != NULL, "fatal: k4c_vector_capacity invalid arguments");

    return k4c_vector->capacity;
}

void *k4c_vector_data(const k4c_vector *k4c_vector) {
    K4C_ASSERT(k4c_vector != NULL, "fatal: k4c_vector_data invalid arguments");

    return k4c_vector->buffer;
}

void *k4c_vector_swap_remove(k4c_vector *k4c_vector, size_t index) {
    K4C_ASSERT(k4c_vector != NULL, "fatal: k4c_vector_swap_remove invalid arguments");

    if (index >= k4c_vector->size) {
        return NULL;
    }

    uint8_t *base = (uint8_t *)k4c_vector->buffer;
    void *slot = base + (index * k4c_vector->elem_size);
    size_t last = k4c_vector->size - 1;

    if (index != last) {
        void *last_slot = base + (last * k4c_vector->elem_size);
        memcpy(slot, last_slot, k4c_vector->elem_size);
    }

    k4c_vector->size = last;
    return slot;
}

size_t k4c_vector_size(const k4c_vector *k4c_vector) {
    K4C_ASSERT(k4c_vector != NULL, "fatal: k4c_vector_size invalid arguments");

    return k4c_vector->size;
}

k4c_iterator k4c_vector_get_iterator(const k4c_vector *k4c_vector) {
    K4C_ASSERT(k4c_vector != NULL, "fatal: k4c_vector_get_iterator invalid arguments");

    k4c_iterator iter = k4c_iterator_from_state(k4c_vector_iterator_next);
    k4c_vector_iterator_state *state = k4c_iterator_state(&iter);
    state->cursor = (const uint8_t *)k4c_vector->buffer;
    state->elem_size = k4c_vector->elem_size;
    state->remaining = k4c_vector->size;
    k4c_iterator_set_size_hint(&iter, k4c_vector->size);
    return iter;
}

k4c_status k4c_vector_collect(
    k4c_iterator *source,
    size_t elem_size,
    k4c_allocator *k4c_allocator,
    k4c_vector **out
) {
    K4C_ASSERT(source != NULL, "fatal: k4c_vector_collect invalid arguments");
    K4C_ASSERT(elem_size > 0, "fatal: k4c_vector_collect invalid arguments");
    K4C_ASSERT(out != NULL, "fatal: k4c_vector_collect invalid arguments");

    *out = NULL;

    k4c_vector *k4c_vector = NULL;
    k4c_status st = k4c_vector_create(elem_size, k4c_allocator, &k4c_vector);
    if (st != K4C_STATUS_OK) {
        return st;
    }
    if (source->size_hint > 0) {
        st = k4c_vector_reserve(k4c_vector, source->size_hint);
        if (st != K4C_STATUS_OK) {
            k4c_vector_destroy(k4c_vector);
            return st;
        }
    }
    const void *item;
    while ((item = k4c_iterator_next(source)) != NULL) {
        st = k4c_vector_push(k4c_vector, item);
        if (st != K4C_STATUS_OK) {
            k4c_vector_destroy(k4c_vector);
            return st;
        }
    }

    *out = k4c_vector;
    return K4C_STATUS_OK;
}

k4c_status k4c_vector_collect_map(
    k4c_iterator *source,
    size_t dst_elem_size,
    k4c_vector_collect_map_fn map,
    void *context,
    k4c_allocator *k4c_allocator,
    k4c_vector **out
) {
    K4C_ASSERT(source != NULL, "fatal: k4c_vector_collect_map invalid arguments");
    K4C_ASSERT(dst_elem_size > 0, "fatal: k4c_vector_collect_map invalid arguments");
    K4C_ASSERT(map != NULL, "fatal: k4c_vector_collect_map invalid arguments");
    K4C_ASSERT(out != NULL, "fatal: k4c_vector_collect_map invalid arguments");

    *out = NULL;

    k4c_vector *k4c_vector = NULL;
    k4c_status st = k4c_vector_create(dst_elem_size, k4c_allocator, &k4c_vector);
    if (st != K4C_STATUS_OK) {
        return st;
    }
    if (source->size_hint > 0) {
        st = k4c_vector_reserve(k4c_vector, source->size_hint);
        if (st != K4C_STATUS_OK) {
            k4c_vector_destroy(k4c_vector);
            return st;
        }
    }
    void *dst = NULL;
    st = k4c_alloc(k4c_allocator, dst_elem_size, &dst);
    if (st != K4C_STATUS_OK) {
        k4c_vector_destroy(k4c_vector);
        return st;
    }

    const void *item;
    while ((item = k4c_iterator_next(source)) != NULL) {
        map(context, item, dst);
        st = k4c_vector_push(k4c_vector, dst);
        if (st != K4C_STATUS_OK) {
            k4c_dealloc(k4c_allocator, dst);
            k4c_vector_destroy(k4c_vector);
            return st;
        }
    }
    k4c_dealloc(k4c_allocator, dst);

    *out = k4c_vector;
    return K4C_STATUS_OK;
}

size_t k4c_vector_lower_bound(
    const k4c_vector *k4c_vector,
    const void *key,
    k4c_vector_cmp_fn cmp
) {
    K4C_ASSERT(k4c_vector != NULL, "fatal: k4c_vector_lower_bound invalid arguments");
    K4C_ASSERT(key != NULL, "fatal: k4c_vector_lower_bound invalid arguments");
    K4C_ASSERT(cmp != NULL, "fatal: k4c_vector_lower_bound invalid arguments");

    size_t first = 0;
    size_t count = k4c_vector->size;

    while (count > 0) {
        size_t step = count / 2;
        size_t mid = first + step;
        const void *item = k4c_vector_get(k4c_vector, mid);

        if (cmp(item, key) < 0) {
            first = mid + 1;
            count -= step + 1;
        } else {
            count = step;
        }
    }

    return first;
}

size_t k4c_vector_upper_bound(
    const k4c_vector *k4c_vector,
    const void *key,
    k4c_vector_cmp_fn cmp
) {
    K4C_ASSERT(k4c_vector != NULL, "fatal: k4c_vector_upper_bound invalid arguments");
    K4C_ASSERT(key != NULL, "fatal: k4c_vector_upper_bound invalid arguments");
    K4C_ASSERT(cmp != NULL, "fatal: k4c_vector_upper_bound invalid arguments");

    size_t first = 0;
    size_t count = k4c_vector->size;

    while (count > 0) {
        size_t step = count / 2;
        size_t mid = first + step;
        const void *item = k4c_vector_get(k4c_vector, mid);

        if (cmp(key, item) >= 0) {
            first = mid + 1;
            count -= step + 1;
        } else {
            count = step;
        }
    }

    return first;
}

size_t k4c_vector_binary_search(
    const k4c_vector *k4c_vector,
    const void *key,
    k4c_vector_cmp_fn cmp
) {
    size_t index = k4c_vector_lower_bound(k4c_vector, key, cmp);

    if (index < k4c_vector_size(k4c_vector) && cmp(k4c_vector_get(k4c_vector, index), key) == 0) {
        return index;
    }

    return k4c_vector_size(k4c_vector);
}

void k4c_vector_destroy(k4c_vector *k4c_vector) {
    K4C_ASSERT(k4c_vector != NULL, "fatal: k4c_vector_destroy invalid arguments");

    k4c_allocator *k4c_allocator = k4c_vector->k4c_allocator;
    k4c_dealloc(k4c_allocator, k4c_vector->buffer);
    k4c_dealloc(k4c_allocator, k4c_vector);
}
