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

#include "k4c/allocators/allocator.h"
#include "k4c/allocators/test_allocator.h"

static void *k4c_test_allocator_alloc(void *ctx, size_t size);
static void k4c_test_allocator_dealloc(void *ctx, void *ptr);
static void *k4c_test_allocator_realloc(void *ctx, void *ptr, size_t size);

static const k4c_allocator_vtable k4c_test_allocator_vtable = {
    .alloc = k4c_test_allocator_alloc,
    .resize = k4c_test_allocator_realloc,
    .dealloc = k4c_test_allocator_dealloc,
};

static bool k4c_test_allocator_should_fail(k4c_test_allocator *k4c_test_allocator) {
    if (k4c_test_allocator->fail_after == K4C_TEST_ALLOCATOR_NO_FAILURE) {
        return false;
    }

    if (k4c_test_allocator->fail_after == 0) {
        k4c_test_allocator->failed_allocations += 1;
        return true;
    }

    k4c_test_allocator->fail_after -= 1;
    return false;
}

static void *k4c_test_allocator_alloc(void *ctx, size_t size) {
    k4c_test_allocator *k4c_test_allocator = ctx;

    if (k4c_test_allocator_should_fail(k4c_test_allocator)) {
        return NULL;
    }

    void *ptr = malloc(size);
    if (ptr == NULL) {
        k4c_test_allocator->failed_allocations += 1;
        return NULL;
    }

    k4c_test_allocator->alloc_count += 1;
    k4c_test_allocator->outstanding_allocations += 1;
    return ptr;
}

static void k4c_test_allocator_dealloc(void *ctx, void *ptr) {
    k4c_test_allocator *k4c_test_allocator = ctx;

    if (ptr == NULL) {
        return;
    }

    k4c_test_allocator->dealloc_count += 1;
    if (k4c_test_allocator->outstanding_allocations > 0) {
        k4c_test_allocator->outstanding_allocations -= 1;
    }
    free(ptr);
}

static void *k4c_test_allocator_realloc(void *ctx, void *ptr, size_t size) {
    k4c_test_allocator *k4c_test_allocator = ctx;

    if (ptr == NULL) {
        return k4c_test_allocator_alloc(ctx, size);
    }

    if (size == 0) {
        k4c_test_allocator_dealloc(ctx, ptr);
        return NULL;
    }

    if (k4c_test_allocator_should_fail(k4c_test_allocator)) {
        return NULL;
    }

    void *new_ptr = realloc(ptr, size);
    if (new_ptr == NULL) {
        k4c_test_allocator->failed_allocations += 1;
        return NULL;
    }

    k4c_test_allocator->realloc_count += 1;
    return new_ptr;
}

k4c_allocator k4c_test_allocator_init(k4c_test_allocator *k4c_test_allocator) {
    k4c_test_allocator->alloc_count = 0;
    k4c_test_allocator->realloc_count = 0;
    k4c_test_allocator->dealloc_count = 0;
    k4c_test_allocator->outstanding_allocations = 0;
    k4c_test_allocator->failed_allocations = 0;
    k4c_test_allocator->fail_after = K4C_TEST_ALLOCATOR_NO_FAILURE;
    return (k4c_allocator){
        .ctx = k4c_test_allocator,
        .features = K4C_ALLOCATOR_FEATURE_DEALLOC | K4C_ALLOCATOR_FEATURE_REALLOC,
        .vtable = &k4c_test_allocator_vtable,
    };
}

void k4c_test_allocator_reset_counts(k4c_test_allocator *k4c_test_allocator) {
    k4c_test_allocator->alloc_count = 0;
    k4c_test_allocator->realloc_count = 0;
    k4c_test_allocator->dealloc_count = 0;
    k4c_test_allocator->failed_allocations = 0;
}

bool k4c_test_allocator_is_clean(const k4c_test_allocator *k4c_test_allocator) {
    return k4c_test_allocator->outstanding_allocations == 0;
}
