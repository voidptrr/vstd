#include <stdint.h>
#include <string.h>

#include "ckit/common/panic.h"
#include "ckit/datastruct/string.h"
#include "ckit/memory/allocators/allocator.h"

#define CKIT_STRING_DEFAULT_CAPACITY 16

typedef struct ckit_string_header {
    size_t len;
    size_t capacity;
    ckit_allocator *allocator;
    char buf[];
} ckit_string_header;

static ckit_string_header *ckit_string_header_from_buf(ckit_string string) {
    if (string == NULL) {
        return NULL;
    }

    return (ckit_string_header *)((uint8_t *)string - offsetof(ckit_string_header, buf));
}

static size_t ckit_string_capacity_for(size_t len) {
    size_t capacity = CKIT_STRING_DEFAULT_CAPACITY;

    while (capacity <= len) {
        capacity *= 2;
    }

    return capacity;
}

static ckit_string_header *ckit_string_ensure_capacity(ckit_string *string, size_t len) {
    ckit_string_header *header = ckit_string_header_from_buf(*string);
    if (len < header->capacity) {
        return header;
    }

    size_t new_capacity = ckit_string_capacity_for(len);
    size_t alloc_size = sizeof(*header) + new_capacity;

    ckit_string_header *tmp = ckit_realloc(header->allocator, header, alloc_size);
    tmp->capacity = new_capacity;
    *string = tmp->buf;
    return tmp;
}

ckit_string ckit_string_init(const char *initial, ckit_allocator *allocator) {
    size_t len = initial == NULL ? 0 : strlen(initial);
    size_t capacity = ckit_string_capacity_for(len);
    size_t alloc_size = sizeof(ckit_string_header) + capacity;

    ckit_string_header *header = ckit_malloc(allocator, alloc_size);
    header->len = len;
    header->capacity = capacity;
    header->allocator = allocator;

    if (len > 0) {
        memcpy(header->buf, initial, len);
    }
    header->buf[len] = '\0';

    return header->buf;
}

void ckit_string_append(ckit_string *string, const char *suffix) {
    CKIT_ASSERT(string != NULL, "fatal: ckit_string_append invalid arguments");
    CKIT_ASSERT(*string != NULL, "fatal: ckit_string_append invalid arguments");
    CKIT_ASSERT(suffix != NULL, "fatal: ckit_string_append invalid arguments");

    size_t suffix_len = strlen(suffix);
    if (suffix_len == 0) {
        return;
    }

    ckit_string_header *header = ckit_string_header_from_buf(*string);
    size_t new_len = header->len + suffix_len;
    header = ckit_string_ensure_capacity(string, new_len);

    memcpy(header->buf + header->len, suffix, suffix_len);
    header->len = new_len;
    header->buf[header->len] = '\0';
}

void ckit_string_prepend(ckit_string *string, const char *prefix) {
    CKIT_ASSERT(string != NULL, "fatal: ckit_string_prepend invalid arguments");
    CKIT_ASSERT(*string != NULL, "fatal: ckit_string_prepend invalid arguments");
    CKIT_ASSERT(prefix != NULL, "fatal: ckit_string_prepend invalid arguments");

    size_t prefix_len = strlen(prefix);
    if (prefix_len == 0) {
        return;
    }

    ckit_string_header *header = ckit_string_header_from_buf(*string);
    size_t old_len = header->len;
    size_t new_len = old_len + prefix_len;
    header = ckit_string_ensure_capacity(string, new_len);

    memmove(header->buf + prefix_len, header->buf, old_len + 1);
    memcpy(header->buf, prefix, prefix_len);
    header->len = new_len;
}

bool ckit_string_contains(const ckit_string string, const char *needle) {
    if (string == NULL || needle == NULL) {
        return false;
    }

    return strstr(string, needle) != NULL;
}

bool ckit_string_starts_with(const ckit_string string, const char *prefix) {
    if (string == NULL || prefix == NULL) {
        return false;
    }

    size_t prefix_len = strlen(prefix);
    return strncmp(string, prefix, prefix_len) == 0;
}

bool ckit_string_ends_with(const ckit_string string, const char *suffix) {
    if (string == NULL || suffix == NULL) {
        return false;
    }

    size_t string_len = ckit_string_len(string);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > string_len) {
        return false;
    }

    return memcmp(string + string_len - suffix_len, suffix, suffix_len) == 0;
}

void ckit_string_clear(ckit_string string) {
    CKIT_ASSERT(string != NULL, "fatal: ckit_string_clear invalid arguments");

    ckit_string_header *header = ckit_string_header_from_buf(string);
    header->len = 0;
    header->buf[0] = '\0';
}

void ckit_string_free(ckit_string string) {
    ckit_string_header *header = ckit_string_header_from_buf(string);
    if (header == NULL) {
        return;
    }

    ckit_dealloc(header->allocator, header);
}

size_t ckit_string_len(const ckit_string string) {
    CKIT_ASSERT(string != NULL, "fatal: ckit_string_len invalid arguments");

    ckit_string_header *header = ckit_string_header_from_buf(string);
    return header->len;
}
