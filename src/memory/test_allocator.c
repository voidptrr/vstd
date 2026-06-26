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

#include <stdlib.h>

#include "vstd/memory/allocator.h"
#include "vstd/memory/test_allocator.h"

static bool vs_test_allocator_should_fail(vs_test_allocator *test_allocator) {
    if (test_allocator->fail_after == VS_TEST_ALLOCATOR_NO_FAILURE) {
        return false;
    }

    if (test_allocator->fail_after == 0) {
        test_allocator->failed_allocations += 1;
        return true;
    }

    test_allocator->fail_after -= 1;
    return false;
}

static void *vs_test_allocator_alloc(void *ctx, size_t size) {
    vs_test_allocator *test_allocator = ctx;

    if (vs_test_allocator_should_fail(test_allocator)) {
        return NULL;
    }

    void *ptr = malloc(size);
    if (ptr == NULL) {
        test_allocator->failed_allocations += 1;
        return NULL;
    }

    test_allocator->alloc_count += 1;
    test_allocator->outstanding_allocations += 1;
    return ptr;
}

static void vs_test_allocator_dealloc(void *ctx, void *ptr) {
    vs_test_allocator *test_allocator = ctx;

    if (ptr == NULL) {
        return;
    }

    test_allocator->dealloc_count += 1;
    if (test_allocator->outstanding_allocations > 0) {
        test_allocator->outstanding_allocations -= 1;
    }
    free(ptr);
}

static void *vs_test_allocator_realloc(void *ctx, void *ptr, size_t size) {
    vs_test_allocator *test_allocator = ctx;

    if (ptr == NULL) {
        return vs_test_allocator_alloc(ctx, size);
    }

    if (size == 0) {
        vs_test_allocator_dealloc(ctx, ptr);
        return NULL;
    }

    if (vs_test_allocator_should_fail(test_allocator)) {
        return NULL;
    }

    void *new_ptr = realloc(ptr, size);
    if (new_ptr == NULL) {
        test_allocator->failed_allocations += 1;
        return NULL;
    }

    test_allocator->realloc_count += 1;
    return new_ptr;
}

vs_allocator *vs_test_allocator_init(vs_test_allocator *test_allocator) {
    test_allocator->alloc_count = 0;
    test_allocator->realloc_count = 0;
    test_allocator->dealloc_count = 0;
    test_allocator->outstanding_allocations = 0;
    test_allocator->failed_allocations = 0;
    test_allocator->fail_after = VS_TEST_ALLOCATOR_NO_FAILURE;
    test_allocator->allocator = (vs_allocator){
        .ctx = test_allocator,
        .features = VS_ALLOCATOR_FEATURE_DEALLOC | VS_ALLOCATOR_FEATURE_REALLOC,
        .alloc = vs_test_allocator_alloc,
        .realloc = vs_test_allocator_realloc,
        .dealloc = vs_test_allocator_dealloc,
    };
    return &test_allocator->allocator;
}

void vs_test_allocator_reset_counts(vs_test_allocator *test_allocator) {
    test_allocator->alloc_count = 0;
    test_allocator->realloc_count = 0;
    test_allocator->dealloc_count = 0;
    test_allocator->failed_allocations = 0;
}

bool vs_test_allocator_is_clean(const vs_test_allocator *test_allocator) {
    return test_allocator->outstanding_allocations == 0;
}
