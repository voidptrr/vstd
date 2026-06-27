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
#include "vstd/ds/iterator.h"
#include "vstd/ds/string.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/utils.h"

#define STRING_DEFAULT_CAPACITY 16

typedef struct string_header {
    size_t len;
    size_t capacity;
    allocator *allocator;
    char buf[];
} string_header;

typedef struct string_iterator_state {
    const char *cursor;
    const char *end;
} string_iterator_state;

_Static_assert(
    sizeof(string_iterator_state) <= ITERATOR_STATE_SIZE,
    "string_iterator_state must fit in iterator"
);

static string_header *string_header_from_buf(string string) {
    if (string == NULL) {
        return NULL;
    }

    return (string_header *)((uint8_t *)string - offsetof(string_header, buf));
}

static status string_capacity_grow(size_t capacity, size_t min_capacity, size_t *out) {
    ASSERT(out != NULL, "fatal: string_capacity_grow invalid arguments");

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

static status string_capacity_for(size_t len, size_t *out) {
    ASSERT(out != NULL, "fatal: string_capacity_for invalid arguments");

    size_t min_capacity = 0;
    if (size_add_overflow(len, 1, &min_capacity)) {
        return STATUS_OVERFLOW;
    }

    return string_capacity_grow(STRING_DEFAULT_CAPACITY, min_capacity, out);
}

static status string_ensure_capacity(string *string, size_t len, string_header **out) {
    ASSERT(out != NULL, "fatal: string_ensure_capacity invalid arguments");

    string_header *header = string_header_from_buf(*string);
    if (len < header->capacity) {
        *out = header;
        return STATUS_OK;
    }

    size_t new_capacity = 0;
    RETURN_IF_ERROR(string_capacity_for(len, &new_capacity));

    size_t alloc_size = 0;
    if (size_add_overflow(sizeof(string_header), new_capacity, &alloc_size)) {
        return STATUS_OVERFLOW;
    }

    allocator *allocator = header->allocator;
    string_header *tmp = NULL;
    status st = resize(allocator, header, alloc_size, (void **)&tmp);
    if (st != STATUS_OK) {
        return st;
    }

    tmp->capacity = new_capacity;
    *string = tmp->buf;
    *out = tmp;
    return STATUS_OK;
}

static const void *string_iterator_next(void *context) {
    ASSERT(context != NULL, "fatal: string_iterator_next invalid arguments");

    string_iterator_state *iterator = context;

    if (iterator->cursor == iterator->end) {
        return NULL;
    }

    return iterator->cursor++;
}

status string_create(const char *initial, allocator *allocator, string *out) {
    ASSERT(out != NULL, "fatal: string_create invalid arguments");

    *out = NULL;

    size_t len = initial == NULL ? 0 : strlen(initial);
    size_t capacity = 0;
    RETURN_IF_ERROR(string_capacity_for(len, &capacity));

    size_t alloc_size = 0;
    if (size_add_overflow(sizeof(string_header), capacity, &alloc_size)) {
        return STATUS_OVERFLOW;
    }

    string_header *header = NULL;
    status st = alloc(allocator, alloc_size, (void **)&header);
    if (st != STATUS_OK) {
        return st;
    }

    header->len = len;
    header->capacity = capacity;
    header->allocator = allocator;

    if (len > 0) {
        memcpy(header->buf, initial, len);
    }
    header->buf[len] = '\0';

    *out = header->buf;
    return STATUS_OK;
}

status string_append(string *string, const char *suffix) {
    ASSERT(string != NULL, "fatal: string_append invalid arguments");
    ASSERT(*string != NULL, "fatal: string_append invalid arguments");
    ASSERT(suffix != NULL, "fatal: string_append invalid arguments");

    size_t suffix_len = strlen(suffix);
    if (suffix_len == 0) {
        return STATUS_OK;
    }

    string_header *header = string_header_from_buf(*string);
    size_t new_len = 0;
    if (size_add_overflow(header->len, suffix_len, &new_len)) {
        return STATUS_OVERFLOW;
    }

    status st = string_ensure_capacity(string, new_len, &header);
    if (st != STATUS_OK) {
        return st;
    }

    memcpy(header->buf + header->len, suffix, suffix_len);
    header->len = new_len;
    header->buf[header->len] = '\0';
    return STATUS_OK;
}

status string_prepend(string *string, const char *prefix) {
    ASSERT(string != NULL, "fatal: string_prepend invalid arguments");
    ASSERT(*string != NULL, "fatal: string_prepend invalid arguments");
    ASSERT(prefix != NULL, "fatal: string_prepend invalid arguments");

    size_t prefix_len = strlen(prefix);
    if (prefix_len == 0) {
        return STATUS_OK;
    }

    string_header *header = string_header_from_buf(*string);
    size_t old_len = header->len;
    size_t new_len = 0;
    if (size_add_overflow(old_len, prefix_len, &new_len)) {
        return STATUS_OVERFLOW;
    }

    status st = string_ensure_capacity(string, new_len, &header);
    if (st != STATUS_OK) {
        return st;
    }

    char *buf = header->buf;
    memmove(buf + prefix_len, buf, old_len + 1);
    char old_first = buf[prefix_len];
    memmove(buf, prefix, prefix_len + 1);
    buf[prefix_len] = old_first;

    header->len = new_len;
    return STATUS_OK;
}

bool string_contains(const string string, const char *needle) {
    if (string == NULL || needle == NULL) {
        return false;
    }

    return strstr(string, needle) != NULL;
}

bool string_starts_with(const string string, const char *prefix) {
    if (string == NULL || prefix == NULL) {
        return false;
    }

    size_t prefix_len = strlen(prefix);
    return strncmp(string, prefix, prefix_len) == 0;
}

bool string_ends_with(const string string, const char *suffix) {
    if (string == NULL || suffix == NULL) {
        return false;
    }

    size_t len = string_len(string);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > len) {
        return false;
    }

    return memcmp(string + len - suffix_len, suffix, suffix_len) == 0;
}

void string_clear(string string) {
    ASSERT(string != NULL, "fatal: string_clear invalid arguments");

    string_header *header = string_header_from_buf(string);
    header->len = 0;
    header->buf[0] = '\0';
}

size_t string_len(const string string) {
    ASSERT(string != NULL, "fatal: string_len invalid arguments");

    string_header *header = string_header_from_buf(string);
    return header->len;
}

iterator string_get_iterator(const string string) {
    ASSERT(string != NULL, "fatal: string_get_iterator invalid arguments");

    size_t len = string_len(string);
    iterator iter = iterator_from_state(string_iterator_next);
    string_iterator_state *state = iterator_state(&iter);
    state->cursor = string;
    state->end = string + len;
    iterator_set_size_hint(&iter, len);
    return iter;
}

void string_destroy(string string) {
    ASSERT(string != NULL, "fatal: string_destroy invalid arguments");

    string_header *header = string_header_from_buf(string);
    allocator *allocator = header->allocator;
    dealloc(allocator, header);
}
