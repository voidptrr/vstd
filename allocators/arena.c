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

#include "allocators/size.h"
#include "k4c/allocators/allocator.h"
#include "k4c/allocators/arena.h"
#include "k4c/allocators/utils.h"
#include "k4c/assert.h"
#include "k4c/error.h"

typedef struct k4c_arena_alloc_header {
    size_t size;
} k4c_arena_alloc_header;

struct k4c_arena {
    void *buffer;
    size_t capacity;
    size_t offset;
};

static void *k4c_arena_vtable_alloc(void *ctx, size_t size);
static void *k4c_arena_vtable_realloc(void *ctx, void *ptr, size_t size);

static const k4c_allocator_vtable k4c_arena_allocator_vtable = {
    .alloc = k4c_arena_vtable_alloc,
    .resize = k4c_arena_vtable_realloc,
    .dealloc = NULL,
};

static void *k4c_arena_vtable_alloc(void *ctx, size_t size) {
    k4c_arena *k4c_arena = ctx;

    K4C_ASSERT(k4c_arena != NULL, "fatal: k4c_arena_alloc invalid arguments");
    K4C_ASSERT(k4c_arena->buffer != NULL, "fatal: k4c_arena_alloc invalid k4c_arena");
    K4C_ASSERT(size > 0, "fatal: k4c_arena_alloc invalid arguments");

    if (k4c_align_up_overflow(size, K4C_MEMORY_ALIGN, &size)) {
        return NULL;
    }

    size_t header_size = k4c_align_up(sizeof(k4c_arena_alloc_header), K4C_MEMORY_ALIGN);
    if (size > k4c_arena->capacity || header_size > k4c_arena->capacity - size
        || k4c_arena->offset > k4c_arena->capacity - header_size - size) {
        return NULL;
    }

    uint8_t *ptr = (uint8_t *)k4c_arena->buffer + k4c_arena->offset;
    k4c_arena_alloc_header *header = (k4c_arena_alloc_header *)ptr;
    header->size = size;

    k4c_arena->offset += header_size + size;
    return ptr + header_size;
}

static void *k4c_arena_vtable_realloc(void *ctx, void *ptr, size_t size) {
    k4c_arena *k4c_arena = ctx;

    K4C_ASSERT(k4c_arena != NULL, "fatal: k4c_arena_realloc invalid arguments");
    K4C_ASSERT(k4c_arena->buffer != NULL, "fatal: k4c_arena_realloc invalid k4c_arena");

    if (ptr == NULL) {
        return k4c_arena_vtable_alloc(ctx, size);
    }

    if (size == 0) {
        return NULL;
    }

    if (k4c_align_up_overflow(size, K4C_MEMORY_ALIGN, &size)) {
        return NULL;
    }

    size_t header_size = k4c_align_up(sizeof(k4c_arena_alloc_header), K4C_MEMORY_ALIGN);
    k4c_arena_alloc_header *header = (k4c_arena_alloc_header *)((uint8_t *)ptr - header_size);
    K4C_ASSERT(size >= header->size, "fatal: k4c_arena_realloc cannot shrink allocation");

    if (size == header->size) {
        return ptr;
    }

    void *new_ptr = k4c_arena_vtable_alloc(ctx, size);
    if (new_ptr == NULL) {
        return NULL;
    }

    memcpy(new_ptr, ptr, header->size);
    return new_ptr;
}

k4c_status k4c_arena_create(size_t capacity, k4c_arena **out) {
    K4C_ASSERT(capacity > 0, "fatal: k4c_arena_create invalid capacity");
    K4C_ASSERT(out != NULL, "fatal: k4c_arena_create invalid arguments");

    *out = NULL;

    if (k4c_align_up_overflow(capacity, K4C_MEMORY_ALIGN, &capacity)) {
        return K4C_STATUS_OVERFLOW;
    }

    k4c_arena *k4c_arena = NULL;
    k4c_status st = k4c_alloc(NULL, sizeof(*k4c_arena), (void **)&k4c_arena);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    st = k4c_alloc(NULL, capacity, &k4c_arena->buffer);
    if (st != K4C_STATUS_OK) {
        k4c_dealloc(NULL, k4c_arena);
        return st;
    }

    k4c_arena->capacity = capacity;
    k4c_arena->offset = 0;
    *out = k4c_arena;
    return K4C_STATUS_OK;
}

k4c_allocator k4c_arena_allocator_view(k4c_arena *k4c_arena) {
    K4C_ASSERT(k4c_arena != NULL, "fatal: k4c_arena_allocator invalid arguments");
    K4C_ASSERT(k4c_arena->buffer != NULL, "fatal: k4c_arena_allocator invalid k4c_arena");

    return (k4c_allocator){
        .ctx = k4c_arena,
        .features = K4C_ALLOCATOR_FEATURE_REALLOC | K4C_ALLOCATOR_FEATURE_RESET,
        .vtable = &k4c_arena_allocator_vtable,
    };
}

void k4c_arena_reset(k4c_arena *k4c_arena) {
    K4C_ASSERT(k4c_arena != NULL, "fatal: k4c_arena_reset invalid arguments");
    K4C_ASSERT(k4c_arena->buffer != NULL, "fatal: k4c_arena_reset invalid k4c_arena");

    k4c_arena->offset = 0;
}

size_t k4c_arena_capacity(const k4c_arena *k4c_arena) {
    K4C_ASSERT(k4c_arena != NULL, "fatal: k4c_arena_capacity invalid arguments");
    K4C_ASSERT(k4c_arena->buffer != NULL, "fatal: k4c_arena_capacity invalid k4c_arena");

    return k4c_arena->capacity;
}

size_t k4c_arena_used(const k4c_arena *k4c_arena) {
    K4C_ASSERT(k4c_arena != NULL, "fatal: k4c_arena_used invalid arguments");
    K4C_ASSERT(k4c_arena->buffer != NULL, "fatal: k4c_arena_used invalid k4c_arena");

    return k4c_arena->offset;
}

size_t k4c_arena_available(const k4c_arena *k4c_arena) {
    K4C_ASSERT(k4c_arena != NULL, "fatal: k4c_arena_available invalid arguments");
    K4C_ASSERT(k4c_arena->buffer != NULL, "fatal: k4c_arena_available invalid k4c_arena");

    return k4c_arena->capacity - k4c_arena->offset;
}

void k4c_arena_destroy(k4c_arena *k4c_arena) {
    K4C_ASSERT(k4c_arena != NULL, "fatal: k4c_arena_destroy invalid arguments");

    k4c_dealloc(NULL, k4c_arena->buffer);
    k4c_dealloc(NULL, k4c_arena);
}
