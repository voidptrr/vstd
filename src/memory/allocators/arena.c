#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ckit/common/panic.h"
#include "ckit/memory/allocators/allocator.h"
#include "ckit/memory/allocators/arena.h"
#include "ckit/memory/utils.h"

typedef struct ckit_arena_alloc_header {
    size_t size;
} ckit_arena_alloc_header;

ckit_allocator ckit_arena_init(ckit_arena *arena, size_t capacity) {
    CKIT_ASSERT(arena != NULL, "fatal: ckit_arena_init invalid arguments");

    capacity = ckit_align_up(capacity, CKIT_MEMORY_ALIGN);
    CKIT_ASSERT(capacity > 0, "fatal: ckit_arena_init invalid capacity");

    arena->buffer = ckit_malloc(NULL, capacity);
    arena->capacity = capacity;
    arena->offset = 0;

    ckit_allocator allocator;
    allocator.ctx = arena;
    allocator.features = CKIT_ALLOCATOR_FEATURE_REALLOC | CKIT_ALLOCATOR_FEATURE_RESET;
    allocator.alloc = (ckit_alloc_fn)ckit_arena_alloc;
    allocator.realloc = (ckit_realloc_fn)ckit_arena_realloc;
    allocator.dealloc = NULL;

    return allocator;
}

void *ckit_arena_alloc(ckit_arena *arena, size_t size) {
    CKIT_ASSERT(arena != NULL, "fatal: ckit_arena_alloc invalid arguments");
    CKIT_ASSERT(arena->buffer != NULL, "fatal: ckit_arena_alloc invalid arena");
    CKIT_ASSERT(size > 0, "fatal: ckit_arena_alloc invalid arguments");

    size = ckit_align_up(size, CKIT_MEMORY_ALIGN);
    size_t header_size = ckit_align_up(sizeof(ckit_arena_alloc_header), CKIT_MEMORY_ALIGN);
    if (size > arena->capacity || header_size > arena->capacity - size ||
        arena->offset > arena->capacity - header_size - size) {
        return NULL;
    }

    uint8_t *ptr = (uint8_t *)arena->buffer + arena->offset;
    ckit_arena_alloc_header *header = (ckit_arena_alloc_header *)ptr;
    header->size = size;

    arena->offset += header_size + size;
    return ptr + header_size;
}

void ckit_arena_free(ckit_arena *arena) {
    CKIT_ASSERT(arena != NULL, "fatal: ckit_arena_free invalid arguments");

    free(arena->buffer);
    arena->buffer = NULL;
    arena->capacity = 0;
    arena->offset = 0;
}

void *ckit_arena_realloc(ckit_arena *arena, void *ptr, size_t size) {
    CKIT_ASSERT(arena != NULL, "fatal: ckit_arena_realloc invalid arguments");
    CKIT_ASSERT(arena->buffer != NULL, "fatal: ckit_arena_realloc invalid arena");

    if (ptr == NULL) {
        return ckit_arena_alloc(arena, size);
    }

    if (size == 0) {
        return NULL;
    }

    size_t header_size = ckit_align_up(sizeof(ckit_arena_alloc_header), CKIT_MEMORY_ALIGN);
    ckit_arena_alloc_header *header = (ckit_arena_alloc_header *)((uint8_t *)ptr - header_size);
    size = ckit_align_up(size, CKIT_MEMORY_ALIGN);
    CKIT_ASSERT(size >= header->size, "fatal: ckit_arena_realloc cannot shrink allocation");

    if (size == header->size) {
        return ptr;
    }

    void *new_ptr = ckit_arena_alloc(arena, size);
    if (new_ptr == NULL) {
        return NULL;
    }

    memcpy(new_ptr, ptr, header->size);
    return new_ptr;
}

void ckit_arena_reset(ckit_arena *arena) {
    CKIT_ASSERT(arena != NULL, "fatal: ckit_arena_reset invalid arguments");
    CKIT_ASSERT(arena->buffer != NULL, "fatal: ckit_arena_reset invalid arena");

    arena->offset = 0;
}

size_t ckit_arena_capacity(const ckit_arena *arena) {
    CKIT_ASSERT(arena != NULL, "fatal: ckit_arena_capacity invalid arguments");
    CKIT_ASSERT(arena->buffer != NULL, "fatal: ckit_arena_capacity invalid arena");

    return arena->capacity;
}

size_t ckit_arena_used(const ckit_arena *arena) {
    CKIT_ASSERT(arena != NULL, "fatal: ckit_arena_used invalid arguments");
    CKIT_ASSERT(arena->buffer != NULL, "fatal: ckit_arena_used invalid arena");

    return arena->offset;
}

size_t ckit_arena_available(const ckit_arena *arena) {
    CKIT_ASSERT(arena != NULL, "fatal: ckit_arena_available invalid arguments");
    CKIT_ASSERT(arena->buffer != NULL, "fatal: ckit_arena_available invalid arena");

    return arena->capacity - arena->offset;
}
