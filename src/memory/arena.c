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
#include "vstd/error.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/arena.h"
#include "vstd/memory/utils.h"

typedef struct arena_alloc_header {
    size_t size;
} arena_alloc_header;

struct arena {
    allocator allocator;
    void *buffer;
    size_t capacity;
    size_t offset;
};

void *arena_alloc(arena *arena, size_t size) {
    ASSERT(arena != NULL, "fatal: arena_alloc invalid arguments");
    ASSERT(arena->buffer != NULL, "fatal: arena_alloc invalid arena");
    ASSERT(size > 0, "fatal: arena_alloc invalid arguments");

    if (align_up_overflow(size, MEMORY_ALIGN, &size)) {
        return NULL;
    }

    size_t header_size = align_up(sizeof(arena_alloc_header), MEMORY_ALIGN);
    if (size > arena->capacity || header_size > arena->capacity - size
        || arena->offset > arena->capacity - header_size - size) {
        return NULL;
    }

    uint8_t *ptr = (uint8_t *)arena->buffer + arena->offset;
    arena_alloc_header *header = (arena_alloc_header *)ptr;
    header->size = size;

    arena->offset += header_size + size;
    return ptr + header_size;
}

void *arena_realloc(arena *arena, void *ptr, size_t size) {
    ASSERT(arena != NULL, "fatal: arena_realloc invalid arguments");
    ASSERT(arena->buffer != NULL, "fatal: arena_realloc invalid arena");

    if (ptr == NULL) {
        return arena_alloc(arena, size);
    }

    if (size == 0) {
        return NULL;
    }

    size_t header_size = align_up(sizeof(arena_alloc_header), MEMORY_ALIGN);
    arena_alloc_header *header = (arena_alloc_header *)((uint8_t *)ptr - header_size);
    if (align_up_overflow(size, MEMORY_ALIGN, &size)) {
        return NULL;
    }

    ASSERT(size >= header->size, "fatal: arena_realloc cannot shrink allocation");

    if (size == header->size) {
        return ptr;
    }

    void *new_ptr = arena_alloc(arena, size);
    if (new_ptr == NULL) {
        return NULL;
    }

    memcpy(new_ptr, ptr, header->size);
    return new_ptr;
}

static void *arena_alloc_callback(void *ctx, size_t size) {
    return arena_alloc(ctx, size);
}

static void *arena_realloc_callback(void *ctx, void *ptr, size_t size) {
    return arena_realloc(ctx, ptr, size);
}

status arena_create(size_t capacity, arena **out) {
    if (align_up_overflow(capacity, MEMORY_ALIGN, &capacity)) {
        return STATUS_OVERFLOW;
    }

    ASSERT(capacity > 0, "fatal: arena_create invalid capacity");
    ASSERT(out != NULL, "fatal: arena_create invalid arguments");

    *out = NULL;

    arena *arena = NULL;
    status st = alloc(NULL, sizeof(*arena), (void **)&arena);
    if (st != STATUS_OK) {
        return st;
    }

    st = alloc(NULL, capacity, &arena->buffer);
    if (st != STATUS_OK) {
        dealloc(NULL, arena);
        return st;
    }

    arena->capacity = capacity;
    arena->offset = 0;
    arena->allocator = (allocator){
        .ctx = arena,
        .features = ALLOCATOR_FEATURE_REALLOC | ALLOCATOR_FEATURE_RESET,
        .alloc = arena_alloc_callback,
        .realloc = arena_realloc_callback,
        .dealloc = NULL,
    };

    *out = arena;
    return STATUS_OK;
}

allocator *arena_allocator(arena *arena) {
    ASSERT(arena != NULL, "fatal: arena_allocator invalid arguments");
    ASSERT(arena->buffer != NULL, "fatal: arena_allocator invalid arena");

    return &arena->allocator;
}

void arena_reset(arena *arena) {
    ASSERT(arena != NULL, "fatal: arena_reset invalid arguments");
    ASSERT(arena->buffer != NULL, "fatal: arena_reset invalid arena");

    arena->offset = 0;
}

size_t arena_capacity(const arena *arena) {
    ASSERT(arena != NULL, "fatal: arena_capacity invalid arguments");
    ASSERT(arena->buffer != NULL, "fatal: arena_capacity invalid arena");

    return arena->capacity;
}

size_t arena_used(const arena *arena) {
    ASSERT(arena != NULL, "fatal: arena_used invalid arguments");
    ASSERT(arena->buffer != NULL, "fatal: arena_used invalid arena");

    return arena->offset;
}

size_t arena_available(const arena *arena) {
    ASSERT(arena != NULL, "fatal: arena_available invalid arguments");
    ASSERT(arena->buffer != NULL, "fatal: arena_available invalid arena");

    return arena->capacity - arena->offset;
}

void arena_destroy(arena *arena) {
    ASSERT(arena != NULL, "fatal: arena_destroy invalid arguments");

    dealloc(NULL, arena->buffer);
    dealloc(NULL, arena);
}
