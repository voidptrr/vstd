#include <stdio.h>

#include "ckit/memory/allocators/arena.h"

int main(void) {
    ckit_arena arena;
    ckit_allocator allocator = ckit_arena_init(&arena, 128);

    if (allocator.ctx != &arena || allocator.alloc == NULL || allocator.realloc == NULL ||
        allocator.dealloc == NULL) {
        fprintf(stderr, "arena init should return allocator adapter\n");
        ckit_arena_free(&arena);
        return 1;
    }

    if (ckit_arena_capacity(&arena) < 128 || ckit_arena_used(&arena) != 0) {
        fprintf(stderr, "arena init should set capacity and empty offset\n");
        ckit_arena_free(&arena);
        return 1;
    }

    ckit_arena_free(&arena);
    return 0;
}
