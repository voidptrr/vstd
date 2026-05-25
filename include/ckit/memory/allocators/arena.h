#ifndef CKIT_MEMORY_ARENA_H
#define CKIT_MEMORY_ARENA_H

#include <stddef.h>

#include "ckit/memory/allocators/allocator.h"

/*
 * Arena memory model (inside arena->buffer):
 *
 * +----------------------+----------------------+
 * | used bytes           | available bytes      |
 * +----------------------+----------------------+
 *                        ^
 *                        arena->offset
 *
 * Allocation strategy:
 * - bump offset forward on alloc
 * - individual dealloc is unsupported
 * - reset releases all allocations together
 * - small internal allocation headers are stored before payloads so realloc can
 *   preserve existing bytes
 */
typedef struct ckit_arena {
    /* Base address of the arena-owned contiguous memory region. */
    void *buffer;
    /* Total bytes managed by this arena region. */
    size_t capacity;
    /* Number of bytes currently consumed from buffer. */
    size_t offset;
} ckit_arena;

/*
 * Initialize an arena with `capacity` bytes of internal storage.
 * Returns an allocator adapter that routes through this arena.
 */
ckit_allocator ckit_arena_init(ckit_arena *arena, size_t capacity);

/*
 * Release arena resources and reset arena state.
 */
void ckit_arena_free(ckit_arena *arena);

/*
 * Allocate `size` bytes from arena.
 * Returns NULL when the arena does not have enough available space.
 */
void *ckit_arena_alloc(ckit_arena *arena, size_t size);

/*
 * Grow an arena allocation.
 * NULL ptr allocates. Size 0 is treated like dealloc and returns NULL.
 * Shrinking an existing allocation is invalid.
 */
void *ckit_arena_realloc(ckit_arena *arena, void *ptr, size_t size);

/*
 * Release all arena allocations while keeping the backing buffer.
 */
void ckit_arena_reset(ckit_arena *arena);

/* Total managed bytes. */
size_t ckit_arena_capacity(const ckit_arena *arena);

/* Number of bytes currently consumed. */
size_t ckit_arena_used(const ckit_arena *arena);

/* Number of bytes still available. */
size_t ckit_arena_available(const ckit_arena *arena);

#endif
