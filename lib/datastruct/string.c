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

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "mem/size.h"
#include "k4c/allocators/allocator.h"
#include "k4c/assert.h"
#include "k4c/datastruct/iterator.h"
#include "k4c/datastruct/string.h"
#include "k4c/error.h"

#define K4C_STRING_DEFAULT_CAPACITY 16

typedef struct k4c_string_header {
    size_t len;
    size_t capacity;
    k4c_allocator k4c_allocator;
    char buf[];
} k4c_string_header;

typedef struct k4c_string_iterator_state {
    const char *cursor;
    const char *end;
} k4c_string_iterator_state;

_Static_assert(
    sizeof(k4c_string_iterator_state) <= K4C_ITERATOR_STATE_SIZE,
    "k4c_string_iterator_state must fit in k4c_iterator"
);

static k4c_string_header *k4c_string_header_from_buf(k4c_string k4c_string) {
    if (k4c_string == NULL) {
        return NULL;
    }

    return (k4c_string_header *)((uint8_t *)k4c_string - offsetof(k4c_string_header, buf));
}

static k4c_status k4c_string_capacity_grow(size_t capacity, size_t min_capacity, size_t *out) {
    K4C_ASSERT(out != NULL, "fatal: k4c_string_capacity_grow invalid arguments");

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

static k4c_status k4c_string_capacity_for(size_t len, size_t *out) {
    K4C_ASSERT(out != NULL, "fatal: k4c_string_capacity_for invalid arguments");

    size_t min_capacity = 0;
    if (k4c_size_add_overflow(len, 1, &min_capacity)) {
        return K4C_STATUS_OVERFLOW;
    }

    return k4c_string_capacity_grow(K4C_STRING_DEFAULT_CAPACITY, min_capacity, out);
}

static k4c_status k4c_string_ensure_capacity(
    k4c_string *k4c_string,
    size_t len,
    k4c_string_header **out
) {
    K4C_ASSERT(out != NULL, "fatal: k4c_string_ensure_capacity invalid arguments");

    k4c_string_header *header = k4c_string_header_from_buf(*k4c_string);
    if (len < header->capacity) {
        *out = header;
        return K4C_STATUS_OK;
    }

    size_t new_capacity = 0;
    K4C_RETURN_IF_ERROR(k4c_string_capacity_for(len, &new_capacity));

    size_t alloc_size = 0;
    if (k4c_size_add_overflow(sizeof(k4c_string_header), new_capacity, &alloc_size)) {
        return K4C_STATUS_OVERFLOW;
    }

    k4c_allocator *k4c_allocator = &header->k4c_allocator;
    k4c_string_header *tmp = NULL;
    k4c_status st = k4c_resize(k4c_allocator, header, alloc_size, (void **)&tmp);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    tmp->capacity = new_capacity;
    *k4c_string = tmp->buf;
    *out = tmp;
    return K4C_STATUS_OK;
}

static const void *k4c_string_iterator_next(void *context) {
    K4C_ASSERT(context != NULL, "fatal: k4c_string_iterator_next invalid arguments");

    k4c_string_iterator_state *k4c_iterator = context;

    if (k4c_iterator->cursor == k4c_iterator->end) {
        return NULL;
    }

    return k4c_iterator->cursor++;
}

k4c_status k4c_string_create(const char *initial, k4c_allocator *k4c_allocator, k4c_string *out) {
    K4C_ASSERT(out != NULL, "fatal: k4c_string_create invalid arguments");

    *out = NULL;

    size_t len = initial == NULL ? 0 : strlen(initial);
    size_t capacity = 0;
    K4C_RETURN_IF_ERROR(k4c_string_capacity_for(len, &capacity));

    size_t alloc_size = 0;
    if (k4c_size_add_overflow(sizeof(k4c_string_header), capacity, &alloc_size)) {
        return K4C_STATUS_OVERFLOW;
    }

    k4c_string_header *header = NULL;
    k4c_status st = k4c_alloc(k4c_allocator, alloc_size, (void **)&header);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    header->len = len;
    header->capacity = capacity;
    header->k4c_allocator = k4c_allocator_copy(k4c_allocator);

    if (len > 0) {
        memcpy(header->buf, initial, len);
    }
    header->buf[len] = '\0';

    *out = header->buf;
    return K4C_STATUS_OK;
}

k4c_status k4c_string_append(k4c_string *k4c_string, const char *suffix) {
    K4C_ASSERT(k4c_string != NULL, "fatal: k4c_string_append invalid arguments");
    K4C_ASSERT(*k4c_string != NULL, "fatal: k4c_string_append invalid arguments");
    K4C_ASSERT(suffix != NULL, "fatal: k4c_string_append invalid arguments");

    size_t suffix_len = strlen(suffix);
    if (suffix_len == 0) {
        return K4C_STATUS_OK;
    }

    k4c_string_header *header = k4c_string_header_from_buf(*k4c_string);
    size_t new_len = 0;
    if (k4c_size_add_overflow(header->len, suffix_len, &new_len)) {
        return K4C_STATUS_OVERFLOW;
    }

    k4c_status st = k4c_string_ensure_capacity(k4c_string, new_len, &header);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    memcpy(header->buf + header->len, suffix, suffix_len);
    header->len = new_len;
    header->buf[header->len] = '\0';
    return K4C_STATUS_OK;
}

k4c_status k4c_string_prepend(k4c_string *k4c_string, const char *prefix) {
    K4C_ASSERT(k4c_string != NULL, "fatal: k4c_string_prepend invalid arguments");
    K4C_ASSERT(*k4c_string != NULL, "fatal: k4c_string_prepend invalid arguments");
    K4C_ASSERT(prefix != NULL, "fatal: k4c_string_prepend invalid arguments");

    size_t prefix_len = strlen(prefix);
    if (prefix_len == 0) {
        return K4C_STATUS_OK;
    }

    k4c_string_header *header = k4c_string_header_from_buf(*k4c_string);
    size_t old_len = header->len;
    size_t new_len = 0;
    if (k4c_size_add_overflow(old_len, prefix_len, &new_len)) {
        return K4C_STATUS_OVERFLOW;
    }

    k4c_status st = k4c_string_ensure_capacity(k4c_string, new_len, &header);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    char *buf = header->buf;
    memmove(buf + prefix_len, buf, old_len + 1);
    char old_first = buf[prefix_len];
    memmove(buf, prefix, prefix_len + 1);
    buf[prefix_len] = old_first;

    header->len = new_len;
    return K4C_STATUS_OK;
}

bool k4c_string_contains(const k4c_string k4c_string, const char *needle) {
    if (k4c_string == NULL || needle == NULL) {
        return false;
    }

    return strstr(k4c_string, needle) != NULL;
}

bool k4c_string_starts_with(const k4c_string k4c_string, const char *prefix) {
    if (k4c_string == NULL || prefix == NULL) {
        return false;
    }

    size_t prefix_len = strlen(prefix);
    return strncmp(k4c_string, prefix, prefix_len) == 0;
}

bool k4c_string_ends_with(const k4c_string k4c_string, const char *suffix) {
    if (k4c_string == NULL || suffix == NULL) {
        return false;
    }

    size_t len = k4c_string_len(k4c_string);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > len) {
        return false;
    }

    return memcmp(k4c_string + len - suffix_len, suffix, suffix_len) == 0;
}

void k4c_string_clear(k4c_string k4c_string) {
    K4C_ASSERT(k4c_string != NULL, "fatal: k4c_string_clear invalid arguments");

    k4c_string_header *header = k4c_string_header_from_buf(k4c_string);
    header->len = 0;
    header->buf[0] = '\0';
}

size_t k4c_string_len(const k4c_string k4c_string) {
    K4C_ASSERT(k4c_string != NULL, "fatal: k4c_string_len invalid arguments");

    k4c_string_header *header = k4c_string_header_from_buf(k4c_string);
    return header->len;
}

k4c_iterator k4c_string_get_iterator(const k4c_string k4c_string) {
    K4C_ASSERT(k4c_string != NULL, "fatal: k4c_string_get_iterator invalid arguments");

    size_t len = k4c_string_len(k4c_string);
    k4c_iterator iter = k4c_iterator_from_state(k4c_string_iterator_next);
    k4c_string_iterator_state *state = k4c_iterator_state(&iter);
    state->cursor = k4c_string;
    state->end = k4c_string + len;
    k4c_iterator_set_size_hint(&iter, len);
    return iter;
}

void k4c_string_destroy(k4c_string k4c_string) {
    K4C_ASSERT(k4c_string != NULL, "fatal: k4c_string_destroy invalid arguments");

    k4c_string_header *header = k4c_string_header_from_buf(k4c_string);
    k4c_allocator *k4c_allocator = &header->k4c_allocator;
    k4c_dealloc(k4c_allocator, header);
}
