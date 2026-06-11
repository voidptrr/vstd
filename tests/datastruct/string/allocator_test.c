/*
 * MIT License
 *
 * Copyright (c) 2026 Tommaso Bruno
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
    int status = 0;
    test_allocator_ctx ctx = {0};
    ck_allocator allocator = {
        .ctx = &ctx,
        .features = CK_ALLOCATOR_FEATURE_DEALLOC | CK_ALLOCATOR_FEATURE_REALLOC,
        .alloc = test_alloc,
        .realloc = test_realloc,
        .dealloc = test_dealloc,
    };

    ck_string value = ck_string_init("abc", &allocator);
    if (ctx.alloc_count != 1 || ctx.realloc_count != 0 || ctx.dealloc_count != 0) {
        fprintf(stderr, "string should initialize through custom allocator\n");
        status = 1;
        goto cleanup;
    }

    ck_string_append(&value, "012345678901234567890123456789");
    if (ctx.realloc_count != 1) {
        fprintf(stderr, "string growth should use custom realloc\n");
        status = 1;
        goto cleanup;
    }

    if (ck_string_len(value) != 33 || strcmp(value, "abc012345678901234567890123456789") != 0) {
        fprintf(stderr, "string should keep contents after allocator-backed growth\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_string_deinit(value);
    if (status != 0) {
        return status;
    }

    if (ctx.dealloc_count != 1) {
        fprintf(stderr, "string should free through custom deallocator\n");
        return 1;
    }

    return 0;
}
