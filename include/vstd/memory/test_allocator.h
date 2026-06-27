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

#ifndef TEST_ALLOCATOR_H
#define TEST_ALLOCATOR_H

#include <stdbool.h>
#include <stddef.h>

#include "vstd/memory/allocator.h"

#define TEST_ALLOCATOR_NO_FAILURE ((size_t)-1)

typedef struct test_allocator {
    size_t alloc_count;
    size_t realloc_count;
    size_t dealloc_count;
    size_t outstanding_allocations;
    size_t failed_allocations;
    size_t fail_after;
    allocator allocator;
} test_allocator;

/* Initialize a malloc-backed tracking allocator and return its generic adapter. */
allocator *test_allocator_init(test_allocator *test_allocator);

/* Reset event counters while keeping outstanding allocation state and fail_after. */
void test_allocator_reset_counts(test_allocator *test_allocator);

/* Return whether every tracked allocation has been released. */
bool test_allocator_is_clean(const test_allocator *test_allocator);

#endif
