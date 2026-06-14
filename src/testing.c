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

#include "ckit/memory/allocators/allocator.h"
#include "ckit/testing.h"

static bool ck_test_allocator_should_fail(ck_test_allocator *test_allocator) {
    if (test_allocator->fail_after == CK_TEST_ALLOCATOR_NO_FAILURE) {
        return false;
    }

    if (test_allocator->fail_after == 0) {
        test_allocator->failed_allocations += 1;
        return true;
    }

    test_allocator->fail_after -= 1;
    return false;
}

static void *ck_test_allocator_alloc(void *ctx, size_t size) {
    ck_test_allocator *test_allocator = ctx;

    if (ck_test_allocator_should_fail(test_allocator)) {
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

static void ck_test_allocator_dealloc(void *ctx, void *ptr) {
    ck_test_allocator *test_allocator = ctx;

    if (ptr == NULL) {
        return;
    }

    test_allocator->dealloc_count += 1;
    if (test_allocator->outstanding_allocations > 0) {
        test_allocator->outstanding_allocations -= 1;
    }
    free(ptr);
}

static void *ck_test_allocator_realloc(void *ctx, void *ptr, size_t size) {
    ck_test_allocator *test_allocator = ctx;

    if (ptr == NULL) {
        return ck_test_allocator_alloc(ctx, size);
    }

    if (size == 0) {
        ck_test_allocator_dealloc(ctx, ptr);
        return NULL;
    }

    if (ck_test_allocator_should_fail(test_allocator)) {
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

int ck_test_fail(const char *file, int line, const char *condition, const char *message) {
    if (message == NULL) {
        fprintf(stderr, "%s:%d: assertion failed: %s\n", file, line, condition);
    } else {
        fprintf(stderr, "%s:%d: assertion failed: %s: %s\n", file, line, condition, message);
    }

    return 1;
}

int ck_test_fail_eq(const char *file, int line, const char *actual_expr,
                    const char *expected_expr) {
    fprintf(stderr, "%s:%d: assertion failed: %s == %s\n", file, line, actual_expr, expected_expr);
    return 1;
}

int ck_test_fail_ptr_null(const char *file, int line, const char *expr, const void *ptr) {
    fprintf(stderr, "%s:%d: assertion failed: %s == NULL (actual %p)\n", file, line, expr, ptr);
    return 1;
}

int ck_test_fail_ptr_eq(const char *file, int line, const char *actual_expr,
                        const char *expected_expr, const void *actual, const void *expected) {
    fprintf(stderr, "%s:%d: assertion failed: %s == %s (actual %p, expected %p)\n", file, line,
            actual_expr, expected_expr, actual, expected);
    return 1;
}

int ck_test_fail_str_eq(const char *file, int line, const char *actual_expr,
                        const char *expected_expr, const char *actual, const char *expected) {
    fprintf(stderr, "%s:%d: assertion failed: %s == %s (actual \"%s\", expected \"%s\")\n", file,
            line, actual_expr, expected_expr, actual == NULL ? "(null)" : actual,
            expected == NULL ? "(null)" : expected);
    return 1;
}

bool ck_test_str_eq(const char *actual, const char *expected) {
    if (actual == NULL || expected == NULL) {
        return actual == expected;
    }

    return strcmp(actual, expected) == 0;
}

void ck_test_allocator_init(ck_test_allocator *test_allocator) {
    test_allocator->alloc_count = 0;
    test_allocator->realloc_count = 0;
    test_allocator->dealloc_count = 0;
    test_allocator->outstanding_allocations = 0;
    test_allocator->failed_allocations = 0;
    test_allocator->fail_after = CK_TEST_ALLOCATOR_NO_FAILURE;
    test_allocator->allocator = (ck_allocator){
        .ctx = test_allocator,
        .features = CK_ALLOCATOR_FEATURE_DEALLOC | CK_ALLOCATOR_FEATURE_REALLOC,
        .alloc = ck_test_allocator_alloc,
        .realloc = ck_test_allocator_realloc,
        .dealloc = ck_test_allocator_dealloc,
    };
}

ck_allocator *ck_test_allocator_allocator(ck_test_allocator *test_allocator) {
    return &test_allocator->allocator;
}

void ck_test_allocator_reset_counts(ck_test_allocator *test_allocator) {
    test_allocator->alloc_count = 0;
    test_allocator->realloc_count = 0;
    test_allocator->dealloc_count = 0;
    test_allocator->failed_allocations = 0;
}

bool ck_test_allocator_is_clean(const ck_test_allocator *test_allocator) {
    return test_allocator->outstanding_allocations == 0;
}
