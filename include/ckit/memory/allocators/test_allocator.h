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

#ifndef CK_MEMORY_TEST_ALLOCATOR_H
#define CK_MEMORY_TEST_ALLOCATOR_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/memory/allocators/allocator.h"

#define CK_TEST_ALLOCATOR_NO_FAILURE ((size_t)-1)

typedef struct ck_test_allocator {
    size_t alloc_count;
    size_t realloc_count;
    size_t dealloc_count;
    size_t outstanding_allocations;
    size_t failed_allocations;
    size_t fail_after;
    ck_allocator allocator;
} ck_test_allocator;

/* Initialize a malloc-backed tracking allocator. */
void ck_test_allocator_init(ck_test_allocator *test_allocator);

/* Return the allocator adapter for APIs that accept ck_allocator. */
ck_allocator *ck_test_allocator_adapter(ck_test_allocator *test_allocator);

/* Reset event counters while keeping outstanding allocation state and fail_after. */
void ck_test_allocator_reset_counts(ck_test_allocator *test_allocator);

/* Return whether every tracked allocation has been released. */
bool ck_test_allocator_is_clean(const ck_test_allocator *test_allocator);

#endif
