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

#ifndef CK_TESTING_H
#define CK_TESTING_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/memory/allocators/allocator.h"

#define CK_TEST_ALLOCATOR_NO_FAILURE ((size_t)-1)

#define CK_TEST_ASSERT(condition)                                                                  \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            return ck_test_fail(__FILE__, __LINE__, #condition, NULL);                             \
        }                                                                                          \
    } while (0)

#define CK_TEST_ASSERT_MSG(condition, message)                                                     \
    do {                                                                                           \
        if (!(condition)) {                                                                        \
            return ck_test_fail(__FILE__, __LINE__, #condition, message);                          \
        }                                                                                          \
    } while (0)

#define CK_TEST_ASSERT_EQ(actual, expected)                                                        \
    do {                                                                                           \
        if ((actual) != (expected)) {                                                              \
            return ck_test_fail_eq(__FILE__, __LINE__, #actual, #expected);                        \
        }                                                                                          \
    } while (0)

#define CK_TEST_ASSERT_PTR_NULL(ptr)                                                               \
    do {                                                                                           \
        const void *ck_test_ptr = (ptr);                                                           \
        if (ck_test_ptr != NULL) {                                                                 \
            return ck_test_fail_ptr_null(__FILE__, __LINE__, #ptr, ck_test_ptr);                   \
        }                                                                                          \
    } while (0)

#define CK_TEST_ASSERT_PTR_NOT_NULL(ptr)                                                           \
    do {                                                                                           \
        const void *ck_test_ptr = (ptr);                                                           \
        if (ck_test_ptr == NULL) {                                                                 \
            return ck_test_fail(__FILE__, __LINE__, #ptr " != NULL", NULL);                        \
        }                                                                                          \
    } while (0)

#define CK_TEST_ASSERT_PTR_EQ(actual, expected)                                                    \
    do {                                                                                           \
        const void *ck_test_actual = (actual);                                                     \
        const void *ck_test_expected = (expected);                                                 \
        if (ck_test_actual != ck_test_expected) {                                                  \
            return ck_test_fail_ptr_eq(__FILE__, __LINE__, #actual, #expected, ck_test_actual,     \
                                       ck_test_expected);                                          \
        }                                                                                          \
    } while (0)

#define CK_TEST_ASSERT_PTR_NE(actual, expected)                                                    \
    do {                                                                                           \
        const void *ck_test_actual = (actual);                                                     \
        const void *ck_test_expected = (expected);                                                 \
        if (ck_test_actual == ck_test_expected) {                                                  \
            return ck_test_fail(__FILE__, __LINE__, #actual " != " #expected, NULL);               \
        }                                                                                          \
    } while (0)

#define CK_TEST_ASSERT_STR_EQ(actual, expected)                                                    \
    do {                                                                                           \
        const char *ck_test_actual = (actual);                                                     \
        const char *ck_test_expected = (expected);                                                 \
        if (!ck_test_str_eq(ck_test_actual, ck_test_expected)) {                                   \
            return ck_test_fail_str_eq(__FILE__, __LINE__, #actual, #expected, ck_test_actual,     \
                                       ck_test_expected);                                          \
        }                                                                                          \
    } while (0)

typedef struct ck_test_allocator {
    size_t alloc_count;
    size_t realloc_count;
    size_t dealloc_count;
    size_t outstanding_allocations;
    size_t failed_allocations;
    size_t fail_after;
    ck_allocator allocator;
} ck_test_allocator;

/* Print an assertion failure and return 1 for direct use from test main. */
int ck_test_fail(const char *file, int line, const char *condition, const char *message);

/* Print an equality assertion failure and return 1 for direct use from test main. */
int ck_test_fail_eq(const char *file, int line, const char *actual_expr, const char *expected_expr);

/* Print a non-NULL pointer assertion failure and return 1 for direct use from test main. */
int ck_test_fail_ptr_null(const char *file, int line, const char *expr, const void *ptr);

/* Print a pointer equality assertion failure and return 1 for direct use from test main. */
int ck_test_fail_ptr_eq(const char *file, int line, const char *actual_expr,
                        const char *expected_expr, const void *actual, const void *expected);

/* Print a string equality assertion failure and return 1 for direct use from test main. */
int ck_test_fail_str_eq(const char *file, int line, const char *actual_expr,
                        const char *expected_expr, const char *actual, const char *expected);

/* Return whether two C strings are equal, treating two NULL pointers as equal. */
bool ck_test_str_eq(const char *actual, const char *expected);

/* Initialize a malloc-backed tracking allocator. */
void ck_test_allocator_init(ck_test_allocator *test_allocator);

/* Return the allocator adapter for APIs that accept ck_allocator. */
ck_allocator *ck_test_allocator_allocator(ck_test_allocator *test_allocator);

/* Reset event counters while keeping outstanding allocation state and fail_after. */
void ck_test_allocator_reset_counts(ck_test_allocator *test_allocator);

/* Return whether every tracked allocation has been released. */
bool ck_test_allocator_is_clean(const ck_test_allocator *test_allocator);

#endif
