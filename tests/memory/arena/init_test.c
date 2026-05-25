#include <stdio.h>

#include "ckit/memory/allocators/arena.h"

int main(void) {
    int status = 0;
    ck_arena *arena = ck_arena_init(128);
    ck_allocator allocator = ck_arena_allocator(arena);

    if (allocator.ctx != arena || allocator.alloc == NULL || allocator.realloc == NULL ||
        allocator.dealloc != NULL ||
        allocator.features != (CK_ALLOCATOR_FEATURE_REALLOC | CK_ALLOCATOR_FEATURE_RESET)) {
        fprintf(stderr, "arena allocator should return allocator adapter\n");
        status = 1;
        goto cleanup;
    }

    if (ck_arena_capacity(arena) < 128 || ck_arena_used(arena) != 0) {
        fprintf(stderr, "arena init should set capacity and empty offset\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_arena_deinit(arena);
    return status;
}
