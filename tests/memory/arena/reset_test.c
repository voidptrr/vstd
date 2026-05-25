#include <stdio.h>

#include "ckit/memory/allocators/arena.h"

int main(void) {
    int status = 0;
    ck_arena *arena = ck_arena_init(128);
    void *before;
    void *after;

    before = ck_arena_alloc(arena, 8);
    if (before == NULL || ck_arena_used(arena) == 0) {
        fprintf(stderr, "arena alloc should consume capacity\n");
        status = 1;
        goto cleanup;
    }

    ck_arena_reset(arena);
    if (ck_arena_used(arena) != 0) {
        fprintf(stderr, "arena reset should clear used bytes\n");
        status = 1;
        goto cleanup;
    }

    after = ck_arena_alloc(arena, 8);
    if (after != before) {
        fprintf(stderr, "arena reset should allow memory reuse\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_arena_deinit(arena);
    return status;
}
