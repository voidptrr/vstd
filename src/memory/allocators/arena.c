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

#include "ckit/common/panic.h"
#include "ckit/memory/allocators/allocator.h"
#include "ckit/memory/allocators/arena.h"
#include "memory/utils.h"

typedef struct ck_arena_alloc_header {
    size_t size;
} ck_arena_alloc_header;

struct ck_arena {
    void *buffer;
    size_t capacity;
    size_t offset;
};

ck_arena *ck_arena_create(size_t capacity) {
    ck_arena *arena;

    capacity = ck_align_up(capacity, CK_MEMORY_ALIGN);
    CK_ASSERT(capacity > 0, "fatal: ck_arena_create invalid capacity");

    arena = ck_malloc(NULL, sizeof(ck_arena));
    arena->buffer = ck_malloc(NULL, capacity);
    arena->capacity = capacity;
    arena->offset = 0;

    return arena;
}

ck_allocator ck_arena_allocator(ck_arena *arena) {
    CK_ASSERT(arena != NULL, "fatal: ck_arena_allocator invalid arguments");
    CK_ASSERT(arena->buffer != NULL, "fatal: ck_arena_allocator invalid arena");

    ck_allocator allocator;
    allocator.ctx = arena;
    allocator.features = CK_ALLOCATOR_FEATURE_REALLOC | CK_ALLOCATOR_FEATURE_RESET;
    allocator.alloc = (ck_alloc_fn)ck_arena_alloc;
    allocator.realloc = (ck_realloc_fn)ck_arena_realloc;
    allocator.dealloc = NULL;

    return allocator;
}

void *ck_arena_alloc(ck_arena *arena, size_t size) {
    CK_ASSERT(arena != NULL, "fatal: ck_arena_alloc invalid arguments");
    CK_ASSERT(arena->buffer != NULL, "fatal: ck_arena_alloc invalid arena");
    CK_ASSERT(size > 0, "fatal: ck_arena_alloc invalid arguments");

    size = ck_align_up(size, CK_MEMORY_ALIGN);
    size_t header_size = ck_align_up(sizeof(ck_arena_alloc_header), CK_MEMORY_ALIGN);
    if (size > arena->capacity || header_size > arena->capacity - size ||
        arena->offset > arena->capacity - header_size - size) {
        return NULL;
    }

    uint8_t *ptr = (uint8_t *)arena->buffer + arena->offset;
    ck_arena_alloc_header *header = (ck_arena_alloc_header *)ptr;
    header->size = size;

    arena->offset += header_size + size;
    return ptr + header_size;
}

void *ck_arena_realloc(ck_arena *arena, void *ptr, size_t size) {
    CK_ASSERT(arena != NULL, "fatal: ck_arena_realloc invalid arguments");
    CK_ASSERT(arena->buffer != NULL, "fatal: ck_arena_realloc invalid arena");

    if (ptr == NULL) {
        return ck_arena_alloc(arena, size);
    }

    if (size == 0) {
        return NULL;
    }

    size_t header_size = ck_align_up(sizeof(ck_arena_alloc_header), CK_MEMORY_ALIGN);
    ck_arena_alloc_header *header = (ck_arena_alloc_header *)((uint8_t *)ptr - header_size);
    size = ck_align_up(size, CK_MEMORY_ALIGN);
    CK_ASSERT(size >= header->size, "fatal: ck_arena_realloc cannot shrink allocation");

    if (size == header->size) {
        return ptr;
    }

    void *new_ptr = ck_arena_alloc(arena, size);
    if (new_ptr == NULL) {
        return NULL;
    }

    memcpy(new_ptr, ptr, header->size);
    return new_ptr;
}

void ck_arena_reset(ck_arena *arena) {
    CK_ASSERT(arena != NULL, "fatal: ck_arena_reset invalid arguments");
    CK_ASSERT(arena->buffer != NULL, "fatal: ck_arena_reset invalid arena");

    arena->offset = 0;
}

size_t ck_arena_capacity(const ck_arena *arena) {
    CK_ASSERT(arena != NULL, "fatal: ck_arena_capacity invalid arguments");
    CK_ASSERT(arena->buffer != NULL, "fatal: ck_arena_capacity invalid arena");

    return arena->capacity;
}

size_t ck_arena_used(const ck_arena *arena) {
    CK_ASSERT(arena != NULL, "fatal: ck_arena_used invalid arguments");
    CK_ASSERT(arena->buffer != NULL, "fatal: ck_arena_used invalid arena");

    return arena->offset;
}

size_t ck_arena_available(const ck_arena *arena) {
    CK_ASSERT(arena != NULL, "fatal: ck_arena_available invalid arguments");
    CK_ASSERT(arena->buffer != NULL, "fatal: ck_arena_available invalid arena");

    return arena->capacity - arena->offset;
}

void ck_arena_destroy(ck_arena *arena) {
    CK_ASSERT(arena != NULL, "fatal: ck_arena_destroy invalid arguments");

    ck_dealloc(NULL, arena->buffer);
    ck_dealloc(NULL, arena);
}
