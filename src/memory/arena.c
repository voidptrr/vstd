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

typedef struct vs_arena_alloc_header {
    size_t size;
} vs_arena_alloc_header;

struct vs_arena {
    vs_allocator allocator;
    void *buffer;
    size_t capacity;
    size_t offset;
};

void *vs_arena_alloc(vs_arena *arena, size_t size) {
    VSTD_ASSERT(arena != NULL, "fatal: vs_arena_alloc invalid arguments");
    VSTD_ASSERT(arena->buffer != NULL, "fatal: vs_arena_alloc invalid arena");
    VSTD_ASSERT(size > 0, "fatal: vs_arena_alloc invalid arguments");

    if (vs_align_up_overflow(size, VS_MEMORY_ALIGN, &size)) {
        return NULL;
    }

    size_t header_size = vs_align_up(sizeof(vs_arena_alloc_header), VS_MEMORY_ALIGN);
    if (size > arena->capacity || header_size > arena->capacity - size
        || arena->offset > arena->capacity - header_size - size) {
        return NULL;
    }

    uint8_t *ptr = (uint8_t *)arena->buffer + arena->offset;
    vs_arena_alloc_header *header = (vs_arena_alloc_header *)ptr;
    header->size = size;

    arena->offset += header_size + size;
    return ptr + header_size;
}

void *vs_arena_realloc(vs_arena *arena, void *ptr, size_t size) {
    VSTD_ASSERT(arena != NULL, "fatal: vs_arena_realloc invalid arguments");
    VSTD_ASSERT(arena->buffer != NULL, "fatal: vs_arena_realloc invalid arena");

    if (ptr == NULL) {
        return vs_arena_alloc(arena, size);
    }

    if (size == 0) {
        return NULL;
    }

    size_t header_size = vs_align_up(sizeof(vs_arena_alloc_header), VS_MEMORY_ALIGN);
    vs_arena_alloc_header *header = (vs_arena_alloc_header *)((uint8_t *)ptr - header_size);
    if (vs_align_up_overflow(size, VS_MEMORY_ALIGN, &size)) {
        return NULL;
    }

    VSTD_ASSERT(size >= header->size, "fatal: vs_arena_realloc cannot shrink allocation");

    if (size == header->size) {
        return ptr;
    }

    void *new_ptr = vs_arena_alloc(arena, size);
    if (new_ptr == NULL) {
        return NULL;
    }

    memcpy(new_ptr, ptr, header->size);
    return new_ptr;
}

static void *vs_arena_alloc_callback(void *ctx, size_t size) {
    return vs_arena_alloc(ctx, size);
}

static void *vs_arena_realloc_callback(void *ctx, void *ptr, size_t size) {
    return vs_arena_realloc(ctx, ptr, size);
}

vs_status vs_arena_create(size_t capacity, vs_arena **out) {
    if (vs_align_up_overflow(capacity, VS_MEMORY_ALIGN, &capacity)) {
        return VS_STATUS_OVERFLOW;
    }

    VSTD_ASSERT(capacity > 0, "fatal: vs_arena_create invalid capacity");
    VSTD_ASSERT(out != NULL, "fatal: vs_arena_create invalid arguments");

    *out = NULL;

    vs_arena *arena = NULL;
    vs_status status = vs_alloc(NULL, sizeof(vs_arena), (void **)&arena);
    if (status != VS_STATUS_OK) {
        return status;
    }

    status = vs_alloc(NULL, capacity, &arena->buffer);
    if (status != VS_STATUS_OK) {
        vs_dealloc(NULL, arena);
        return status;
    }

    arena->capacity = capacity;
    arena->offset = 0;
    arena->allocator = (vs_allocator){
        .ctx = arena,
        .features = VS_ALLOCATOR_FEATURE_REALLOC | VS_ALLOCATOR_FEATURE_RESET,
        .alloc = vs_arena_alloc_callback,
        .realloc = vs_arena_realloc_callback,
        .dealloc = NULL,
    };

    *out = arena;
    return VS_STATUS_OK;
}

vs_allocator *vs_arena_allocator(vs_arena *arena) {
    VSTD_ASSERT(arena != NULL, "fatal: vs_arena_allocator invalid arguments");
    VSTD_ASSERT(arena->buffer != NULL, "fatal: vs_arena_allocator invalid arena");

    return &arena->allocator;
}

void vs_arena_reset(vs_arena *arena) {
    VSTD_ASSERT(arena != NULL, "fatal: vs_arena_reset invalid arguments");
    VSTD_ASSERT(arena->buffer != NULL, "fatal: vs_arena_reset invalid arena");

    arena->offset = 0;
}

size_t vs_arena_capacity(const vs_arena *arena) {
    VSTD_ASSERT(arena != NULL, "fatal: vs_arena_capacity invalid arguments");
    VSTD_ASSERT(arena->buffer != NULL, "fatal: vs_arena_capacity invalid arena");

    return arena->capacity;
}

size_t vs_arena_used(const vs_arena *arena) {
    VSTD_ASSERT(arena != NULL, "fatal: vs_arena_used invalid arguments");
    VSTD_ASSERT(arena->buffer != NULL, "fatal: vs_arena_used invalid arena");

    return arena->offset;
}

size_t vs_arena_available(const vs_arena *arena) {
    VSTD_ASSERT(arena != NULL, "fatal: vs_arena_available invalid arguments");
    VSTD_ASSERT(arena->buffer != NULL, "fatal: vs_arena_available invalid arena");

    return arena->capacity - arena->offset;
}

void vs_arena_destroy(vs_arena *arena) {
    VSTD_ASSERT(arena != NULL, "fatal: vs_arena_destroy invalid arguments");

    vs_dealloc(NULL, arena->buffer);
    vs_dealloc(NULL, arena);
}
