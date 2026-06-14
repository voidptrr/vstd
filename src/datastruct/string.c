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
#include <stddef.h>
#include <string.h>

#include "ckit/common/panic.h"
#include "ckit/datastruct/string.h"
#include "ckit/memory/allocators/allocator.h"

#define CK_STRING_DEFAULT_CAPACITY 16

typedef struct ck_string_header {
    size_t len;
    size_t capacity;
    ck_allocator *allocator;
    char buf[];
} ck_string_header;

static ck_string_header *ck_string_header_from_buf(ck_string string) {
    if (string == NULL) {
        return NULL;
    }

    return (ck_string_header *)((uint8_t *)string - offsetof(ck_string_header, buf));
}

static size_t ck_string_capacity_for(size_t len) {
    size_t capacity = CK_STRING_DEFAULT_CAPACITY;

    while (capacity <= len) {
        capacity *= 2;
    }

    return capacity;
}

static ck_string_header *ck_string_ensure_capacity(ck_string *string, size_t len) {
    ck_string_header *header = ck_string_header_from_buf(*string);
    if (len < header->capacity) {
        return header;
    }

    size_t new_capacity = ck_string_capacity_for(len);
    size_t alloc_size = sizeof(ck_string_header) + new_capacity;

    ck_string_header *tmp = ck_realloc(header->allocator, header, alloc_size);
    tmp->capacity = new_capacity;
    *string = tmp->buf;
    return tmp;
}

ck_string ck_string_create(const char *initial, ck_allocator *allocator) {
    size_t len = initial == NULL ? 0 : strlen(initial);
    size_t capacity = ck_string_capacity_for(len);
    size_t alloc_size = sizeof(ck_string_header) + capacity;

    ck_string_header *header = ck_malloc(allocator, alloc_size);
    header->len = len;
    header->capacity = capacity;
    header->allocator = allocator;

    if (len > 0) {
        memcpy(header->buf, initial, len);
    }
    header->buf[len] = '\0';

    return header->buf;
}

void ck_string_append(ck_string *string, const char *suffix) {
    CK_ASSERT(string != NULL, "fatal: ck_string_append invalid arguments");
    CK_ASSERT(*string != NULL, "fatal: ck_string_append invalid arguments");
    CK_ASSERT(suffix != NULL, "fatal: ck_string_append invalid arguments");

    size_t suffix_len = strlen(suffix);
    if (suffix_len == 0) {
        return;
    }

    ck_string_header *header = ck_string_header_from_buf(*string);
    size_t new_len = header->len + suffix_len;
    header = ck_string_ensure_capacity(string, new_len);

    memcpy(header->buf + header->len, suffix, suffix_len);
    header->len = new_len;
    header->buf[header->len] = '\0';
}

void ck_string_prepend(ck_string *string, const char *prefix) {
    CK_ASSERT(string != NULL, "fatal: ck_string_prepend invalid arguments");
    CK_ASSERT(*string != NULL, "fatal: ck_string_prepend invalid arguments");
    CK_ASSERT(prefix != NULL, "fatal: ck_string_prepend invalid arguments");

    size_t prefix_len = strlen(prefix);
    if (prefix_len == 0) {
        return;
    }

    ck_string_header *header = ck_string_header_from_buf(*string);
    size_t old_len = header->len;
    size_t new_len = old_len + prefix_len;
    header = ck_string_ensure_capacity(string, new_len);

    memmove(header->buf + prefix_len, header->buf, old_len + 1);
    memcpy(header->buf, prefix, prefix_len);
    header->len = new_len;
}

bool ck_string_contains(const ck_string string, const char *needle) {
    if (string == NULL || needle == NULL) {
        return false;
    }

    return strstr(string, needle) != NULL;
}

bool ck_string_starts_with(const ck_string string, const char *prefix) {
    if (string == NULL || prefix == NULL) {
        return false;
    }

    size_t prefix_len = strlen(prefix);
    return strncmp(string, prefix, prefix_len) == 0;
}

bool ck_string_ends_with(const ck_string string, const char *suffix) {
    if (string == NULL || suffix == NULL) {
        return false;
    }

    size_t string_len = ck_string_len(string);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > string_len) {
        return false;
    }

    return memcmp(string + string_len - suffix_len, suffix, suffix_len) == 0;
}

void ck_string_clear(ck_string string) {
    CK_ASSERT(string != NULL, "fatal: ck_string_clear invalid arguments");

    ck_string_header *header = ck_string_header_from_buf(string);
    header->len = 0;
    header->buf[0] = '\0';
}

size_t ck_string_len(const ck_string string) {
    CK_ASSERT(string != NULL, "fatal: ck_string_len invalid arguments");

    ck_string_header *header = ck_string_header_from_buf(string);
    return header->len;
}

void ck_string_destroy(ck_string string) {
    ck_string_header *header = ck_string_header_from_buf(string);
    if (header == NULL) {
        return;
    }

    ck_dealloc(header->allocator, header);
}
