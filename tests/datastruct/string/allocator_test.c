#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ckit/datastruct/string.h"
#include "ckit/memory/allocators/allocator.h"

typedef struct test_allocator_ctx {
    size_t alloc_count;
    size_t realloc_count;
    size_t dealloc_count;
} test_allocator_ctx;

static void *test_alloc(void *ctx, size_t size) {
    test_allocator_ctx *test_ctx = ctx;
    test_ctx->alloc_count += 1;
    return malloc(size);
}

static void *test_realloc(void *ctx, void *ptr, size_t size) {
    test_allocator_ctx *test_ctx = ctx;
    test_ctx->realloc_count += 1;
    return realloc(ptr, size);
}

static void test_dealloc(void *ctx, void *ptr) {
    test_allocator_ctx *test_ctx = ctx;
    test_ctx->dealloc_count += 1;
    free(ptr);
}

int main(void) {
    test_allocator_ctx ctx = {0};
    ckit_allocator allocator = {
        .ctx = &ctx,
        .features = CKIT_ALLOCATOR_FEATURE_DEALLOC | CKIT_ALLOCATOR_FEATURE_REALLOC,
        .alloc = test_alloc,
        .realloc = test_realloc,
        .dealloc = test_dealloc,
    };

    ckit_string value = ckit_string_init("abc", &allocator);
    if (ctx.alloc_count != 1 || ctx.realloc_count != 0 || ctx.dealloc_count != 0) {
        fprintf(stderr, "string should initialize through custom allocator\n");
        ckit_string_deinit(value);
        return 1;
    }

    ckit_string_append(&value, "012345678901234567890123456789");
    if (ctx.realloc_count != 1) {
        fprintf(stderr, "string growth should use custom realloc\n");
        ckit_string_deinit(value);
        return 1;
    }

    if (ckit_string_len(value) != 33 || strcmp(value, "abc012345678901234567890123456789") != 0) {
        fprintf(stderr, "string should keep contents after allocator-backed growth\n");
        ckit_string_deinit(value);
        return 1;
    }

    ckit_string_deinit(value);
    if (ctx.dealloc_count != 1) {
        fprintf(stderr, "string should free through custom deallocator\n");
        return 1;
    }

    return 0;
}
