#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashmap.h"
#include "ckit/memory/allocators/allocator.h"

typedef struct test_allocator_ctx {
    size_t alloc_count;
    size_t dealloc_count;
} test_allocator_ctx;

static void *test_alloc(void *ctx, size_t size) {
    test_allocator_ctx *test_ctx = ctx;
    test_ctx->alloc_count += 1;
    return malloc(size);
}

static void test_dealloc(void *ctx, void *ptr) {
    test_allocator_ctx *test_ctx = ctx;
    test_ctx->dealloc_count += 1;
    free(ptr);
}

int main(void) {
    int status = 0;
    test_allocator_ctx ctx = {0};
    ck_allocator allocator = {
        .ctx = &ctx,
        .features = CK_ALLOCATOR_FEATURE_DEALLOC,
        .alloc = test_alloc,
        .realloc = NULL,
        .dealloc = test_dealloc,
    };
    uint64_t key = 7;
    uint64_t value = 11;

    ck_hashmap *map = ck_hashmap_init(sizeof(uint64_t), sizeof(uint64_t), ck_eq_u64, &allocator);
    ctx.alloc_count = 0;

    ck_hashmap_put(map, &key, &value);
    if (ctx.alloc_count != 1) {
        fprintf(stderr, "hashmap insert should allocate one entry with inline key/value storage\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_hashmap_deinit(map);
    return status;
}
