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

#include "vstd/assert.h"
#include "vstd/datastruct/iterator.h"
#include "vstd/datastruct/string.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/utils.h"

#define VS_STRING_DEFAULT_CAPACITY 16

typedef struct vs_string_header {
    size_t len;
    size_t capacity;
    vs_allocator *allocator;
    char buf[];
} vs_string_header;

typedef struct vs_string_iterator_state {
    const char *cursor;
    const char *end;
} vs_string_iterator_state;

_Static_assert(
    sizeof(vs_string_iterator_state) <= VS_ITERATOR_STATE_SIZE,
    "vs_string_iterator_state must fit in vs_iterator"
);

static vs_string_header *vs_string_header_from_buf(vs_string string) {
    if (string == NULL) {
        return NULL;
    }

    return (vs_string_header *)((uint8_t *)string - offsetof(vs_string_header, buf));
}

static vs_status vs_string_capacity_grow(size_t capacity, size_t min_capacity, size_t *out) {
    VSTD_ASSERT(out != NULL, "fatal: vs_string_capacity_grow invalid arguments");

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

static vs_status vs_string_capacity_for(size_t len, size_t *out) {
    VSTD_ASSERT(out != NULL, "fatal: vs_string_capacity_for invalid arguments");

    size_t min_capacity = 0;
    if (vs_size_add_overflow(len, 1, &min_capacity)) {
        return VS_STATUS_OVERFLOW;
    }

    return vs_string_capacity_grow(VS_STRING_DEFAULT_CAPACITY, min_capacity, out);
}

static vs_status vs_string_ensure_capacity(vs_string *string, size_t len, vs_string_header **out) {
    VSTD_ASSERT(out != NULL, "fatal: vs_string_ensure_capacity invalid arguments");

    vs_string_header *header = vs_string_header_from_buf(*string);
    if (len < header->capacity) {
        *out = header;
        return VS_STATUS_OK;
    }

    size_t new_capacity = 0;
    VS_RETURN_IF_ERROR(vs_string_capacity_for(len, &new_capacity));

    size_t alloc_size = 0;
    if (vs_size_add_overflow(sizeof(vs_string_header), new_capacity, &alloc_size)) {
        return VS_STATUS_OVERFLOW;
    }

    vs_allocator *allocator = header->allocator;
    vs_string_header *tmp = NULL;
    vs_status status = vs_resize(allocator, header, alloc_size, (void **)&tmp);
    if (status != VS_STATUS_OK) {
        return status;
    }

    tmp->capacity = new_capacity;
    *string = tmp->buf;
    *out = tmp;
    return VS_STATUS_OK;
}

static const void *vs_string_iterator_next(void *context) {
    VSTD_ASSERT(context != NULL, "fatal: vs_string_iterator_next invalid arguments");

    vs_string_iterator_state *iterator = context;

    if (iterator->cursor == iterator->end) {
        return NULL;
    }

    return iterator->cursor++;
}

vs_status vs_string_create(const char *initial, vs_allocator *allocator, vs_string *out) {
    VSTD_ASSERT(out != NULL, "fatal: vs_string_create invalid arguments");

    *out = NULL;

    size_t len = initial == NULL ? 0 : strlen(initial);
    size_t capacity = 0;
    VS_RETURN_IF_ERROR(vs_string_capacity_for(len, &capacity));

    size_t alloc_size = 0;
    if (vs_size_add_overflow(sizeof(vs_string_header), capacity, &alloc_size)) {
        return VS_STATUS_OVERFLOW;
    }

    vs_string_header *header = NULL;
    vs_status status = vs_alloc(allocator, alloc_size, (void **)&header);
    if (status != VS_STATUS_OK) {
        return status;
    }

    header->len = len;
    header->capacity = capacity;
    header->allocator = allocator;

    if (len > 0) {
        memcpy(header->buf, initial, len);
    }
    header->buf[len] = '\0';

    *out = header->buf;
    return VS_STATUS_OK;
}

vs_status vs_string_append(vs_string *string, const char *suffix) {
    VSTD_ASSERT(string != NULL, "fatal: vs_string_append invalid arguments");
    VSTD_ASSERT(*string != NULL, "fatal: vs_string_append invalid arguments");
    VSTD_ASSERT(suffix != NULL, "fatal: vs_string_append invalid arguments");

    size_t suffix_len = strlen(suffix);
    if (suffix_len == 0) {
        return VS_STATUS_OK;
    }

    vs_string_header *header = vs_string_header_from_buf(*string);
    size_t new_len = 0;
    if (vs_size_add_overflow(header->len, suffix_len, &new_len)) {
        return VS_STATUS_OVERFLOW;
    }

    vs_status status = vs_string_ensure_capacity(string, new_len, &header);
    if (status != VS_STATUS_OK) {
        return status;
    }

    memcpy(header->buf + header->len, suffix, suffix_len);
    header->len = new_len;
    header->buf[header->len] = '\0';
    return VS_STATUS_OK;
}

vs_status vs_string_prepend(vs_string *string, const char *prefix) {
    VSTD_ASSERT(string != NULL, "fatal: vs_string_prepend invalid arguments");
    VSTD_ASSERT(*string != NULL, "fatal: vs_string_prepend invalid arguments");
    VSTD_ASSERT(prefix != NULL, "fatal: vs_string_prepend invalid arguments");

    size_t prefix_len = strlen(prefix);
    if (prefix_len == 0) {
        return VS_STATUS_OK;
    }

    vs_string_header *header = vs_string_header_from_buf(*string);
    size_t old_len = header->len;
    size_t new_len = 0;
    if (vs_size_add_overflow(old_len, prefix_len, &new_len)) {
        return VS_STATUS_OVERFLOW;
    }

    vs_status status = vs_string_ensure_capacity(string, new_len, &header);
    if (status != VS_STATUS_OK) {
        return status;
    }

    char *buf = header->buf;
    memmove(buf + prefix_len, buf, old_len + 1);
    char old_first = buf[prefix_len];
    memmove(buf, prefix, prefix_len + 1);
    buf[prefix_len] = old_first;

    header->len = new_len;
    return VS_STATUS_OK;
}

bool vs_string_contains(const vs_string string, const char *needle) {
    if (string == NULL || needle == NULL) {
        return false;
    }

    return strstr(string, needle) != NULL;
}

bool vs_string_starts_with(const vs_string string, const char *prefix) {
    if (string == NULL || prefix == NULL) {
        return false;
    }

    size_t prefix_len = strlen(prefix);
    return strncmp(string, prefix, prefix_len) == 0;
}

bool vs_string_ends_with(const vs_string string, const char *suffix) {
    if (string == NULL || suffix == NULL) {
        return false;
    }

    size_t string_len = vs_string_len(string);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > string_len) {
        return false;
    }

    return memcmp(string + string_len - suffix_len, suffix, suffix_len) == 0;
}

void vs_string_clear(vs_string string) {
    VSTD_ASSERT(string != NULL, "fatal: vs_string_clear invalid arguments");

    vs_string_header *header = vs_string_header_from_buf(string);
    header->len = 0;
    header->buf[0] = '\0';
}

size_t vs_string_len(const vs_string string) {
    VSTD_ASSERT(string != NULL, "fatal: vs_string_len invalid arguments");

    vs_string_header *header = vs_string_header_from_buf(string);
    return header->len;
}

vs_iterator vs_string_get_iterator(const vs_string string) {
    VSTD_ASSERT(string != NULL, "fatal: vs_string_get_iterator invalid arguments");

    size_t len = vs_string_len(string);
    vs_iterator iter = vs_iterator_from_state(vs_string_iterator_next);
    vs_string_iterator_state *state = vs_iterator_state(&iter);
    state->cursor = string;
    state->end = string + len;
    vs_iterator_set_size_hint(&iter, len);
    return iter;
}

void vs_string_destroy(vs_string string) {
    VSTD_ASSERT(string != NULL, "fatal: vs_string_destroy invalid arguments");

    vs_string_header *header = vs_string_header_from_buf(string);
    vs_allocator *allocator = header->allocator;
    vs_dealloc(allocator, header);
}
