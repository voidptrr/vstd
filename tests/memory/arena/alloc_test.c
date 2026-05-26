#include <stdint.h>
#include <stdio.h>

#include "ckit/memory/allocators/arena.h"
#include "memory/utils.h"

int main(void) {
    int status = 0;
    ck_arena *arena = ck_arena_init(128);
    void *first;
    void *second;

    first = ck_arena_alloc(arena, 8);
    second = ck_arena_alloc(arena, 8);
    if (first == NULL || second == NULL || first == second) {
        fprintf(stderr, "arena alloc should return distinct allocations\n");
        status = 1;
        goto cleanup;
    }

    if (((uintptr_t)first % CK_MEMORY_ALIGN) != 0 || ((uintptr_t)second % CK_MEMORY_ALIGN) != 0) {
        fprintf(stderr, "arena alloc should return aligned pointers\n");
        status = 1;
        goto cleanup;
    }

    if (ck_arena_alloc(arena, ck_arena_capacity(arena)) != NULL) {
        fprintf(stderr, "arena alloc should return NULL when capacity is exceeded\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_arena_deinit(arena);
    return status;
}
