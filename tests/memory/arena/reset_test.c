#include <stdio.h>

#include "ckit/memory/allocators/arena.h"

int main(void) {
    ckit_arena arena;
    void *before;
    void *after;

    (void)ckit_arena_init(&arena, 128);

    before = ckit_arena_alloc(&arena, 8);
    if (before == NULL || ckit_arena_used(&arena) == 0) {
        fprintf(stderr, "arena alloc should consume capacity\n");
        ckit_arena_free(&arena);
        return 1;
    }

    ckit_arena_reset(&arena);
    if (ckit_arena_used(&arena) != 0) {
        fprintf(stderr, "arena reset should clear used bytes\n");
        ckit_arena_free(&arena);
        return 1;
    }

    after = ckit_arena_alloc(&arena, 8);
    if (after != before) {
        fprintf(stderr, "arena reset should allow memory reuse\n");
        ckit_arena_free(&arena);
        return 1;
    }

    ckit_arena_free(&arena);
    return 0;
}
