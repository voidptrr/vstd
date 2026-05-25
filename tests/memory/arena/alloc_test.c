#include <stdint.h>
#include <stdio.h>

#include "ckit/memory/allocators/arena.h"
#include "ckit/memory/utils.h"

int main(void) {
    ckit_arena arena;
    void *first;
    void *second;

    (void)ckit_arena_init(&arena, 128);

    first = ckit_arena_alloc(&arena, 8);
    second = ckit_arena_alloc(&arena, 8);
    if (first == NULL || second == NULL || first == second) {
        fprintf(stderr, "arena alloc should return distinct allocations\n");
        ckit_arena_free(&arena);
        return 1;
    }

    if (((uintptr_t)first % CKIT_MEMORY_ALIGN) != 0 ||
        ((uintptr_t)second % CKIT_MEMORY_ALIGN) != 0) {
        fprintf(stderr, "arena alloc should return aligned pointers\n");
        ckit_arena_free(&arena);
        return 1;
    }

    if (ckit_arena_alloc(&arena, ckit_arena_capacity(&arena)) != NULL) {
        fprintf(stderr, "arena alloc should return NULL when capacity is exceeded\n");
        ckit_arena_free(&arena);
        return 1;
    }

    ckit_arena_free(&arena);
    return 0;
}
