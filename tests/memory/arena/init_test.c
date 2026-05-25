#include <stdio.h>

#include "ckit/memory/allocators/arena.h"

int main(void) {
    ckit_arena *arena = ckit_arena_init(128);
    ckit_allocator allocator = ckit_arena_allocator(arena);

    if (allocator.ctx != arena || allocator.alloc == NULL || allocator.realloc == NULL ||
        allocator.dealloc != NULL ||
        allocator.features != (CKIT_ALLOCATOR_FEATURE_REALLOC | CKIT_ALLOCATOR_FEATURE_RESET)) {
        fprintf(stderr, "arena allocator should return allocator adapter\n");
        ckit_arena_deinit(arena);
        return 1;
    }

    if (ckit_arena_capacity(arena) < 128 || ckit_arena_used(arena) != 0) {
        fprintf(stderr, "arena init should set capacity and empty offset\n");
        ckit_arena_deinit(arena);
        return 1;
    }

    ckit_arena_deinit(arena);
    return 0;
}
