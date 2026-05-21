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

ckit_string ckit_string_init(const char *initial, ckit_allocator *allocator) {
    size_t len = initial == NULL ? 0U : strlen(initial);
    size_t capacity = ckit_string_capacity_for(len);
    size_t alloc_size = sizeof(ckit_string_header) + capacity;

    ckit_string_header *header = ckit_malloc(allocator, alloc_size);
    header->len = len;
    header->capacity = capacity;
    header->allocator = allocator;

    if (len > 0U) {
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
    if (suffix_len == 0U) {
        return;
    }

    ckit_string_header *header = ckit_string_header_from_buf(*string);
    size_t new_len = header->len + suffix_len;

    if (new_len >= header->capacity) {
        size_t new_capacity = ckit_string_capacity_for(new_len);
        size_t alloc_size = sizeof(*header) + new_capacity;

        ckit_string_header *tmp = ckit_realloc(header->allocator, header, alloc_size);
        header = tmp;
        header->capacity = new_capacity;
        *string = header->buf;
    }

    memcpy(header->buf + header->len, suffix, suffix_len);
    header->len = new_len;
    header->buf[header->len] = '\0';
}

void ckit_string_free(ckit_string string) {
    ckit_string_header *header = ckit_string_header_from_buf(string);
    if (header == NULL) {
        return;
    }

    ckit_dealloc(header->allocator, header);
}

size_t ckit_string_len(const ckit_string string) {
    ckit_string_header *header = ckit_string_header_from_buf(string);
    if (header == NULL) {
        return 0;
    }

    return header->len;
}
