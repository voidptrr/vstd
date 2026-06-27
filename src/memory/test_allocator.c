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

static bool test_allocator_should_fail(test_allocator *test_allocator) {
    if (test_allocator->fail_after == TEST_ALLOCATOR_NO_FAILURE) {
        return false;
    }

    if (test_allocator->fail_after == 0) {
        test_allocator->failed_allocations += 1;
        return true;
    }

    test_allocator->fail_after -= 1;
    return false;
}

static void *test_allocator_alloc(void *ctx, size_t size) {
    test_allocator *test_allocator = ctx;

    if (test_allocator_should_fail(test_allocator)) {
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

static void test_allocator_dealloc(void *ctx, void *ptr) {
    test_allocator *test_allocator = ctx;

    if (ptr == NULL) {
        return;
    }

    test_allocator->dealloc_count += 1;
    if (test_allocator->outstanding_allocations > 0) {
        test_allocator->outstanding_allocations -= 1;
    }
    free(ptr);
}

static void *test_allocator_realloc(void *ctx, void *ptr, size_t size) {
    test_allocator *test_allocator = ctx;

    if (ptr == NULL) {
        return test_allocator_alloc(ctx, size);
    }

    if (size == 0) {
        test_allocator_dealloc(ctx, ptr);
        return NULL;
    }

    if (test_allocator_should_fail(test_allocator)) {
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

allocator *test_allocator_init(test_allocator *test_allocator) {
    test_allocator->alloc_count = 0;
    test_allocator->realloc_count = 0;
    test_allocator->dealloc_count = 0;
    test_allocator->outstanding_allocations = 0;
    test_allocator->failed_allocations = 0;
    test_allocator->fail_after = TEST_ALLOCATOR_NO_FAILURE;
    test_allocator->allocator = (allocator){
        .ctx = test_allocator,
        .features = ALLOCATOR_FEATURE_DEALLOC | ALLOCATOR_FEATURE_REALLOC,
        .alloc = test_allocator_alloc,
        .realloc = test_allocator_realloc,
        .dealloc = test_allocator_dealloc,
    };
    return &test_allocator->allocator;
}

void test_allocator_reset_counts(test_allocator *test_allocator) {
    test_allocator->alloc_count = 0;
    test_allocator->realloc_count = 0;
    test_allocator->dealloc_count = 0;
    test_allocator->failed_allocations = 0;
}

bool test_allocator_is_clean(const test_allocator *test_allocator) {
    return test_allocator->outstanding_allocations == 0;
}
