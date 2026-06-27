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

#ifndef TESTING_H
#define TESTING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define TEST(name) static int test_case_##name(void)
#define TEST_CASE(name) {#name, test_case_##name}
#define TEST_MAIN(...) \
    int main(void) { \
        static const test_case test_cases[] = {__VA_ARGS__}; \
        return test_run(test_cases, sizeof(test_cases) / sizeof(test_cases[0])); \
    }

#define test_equal(actual, expected) test_equal_intmax((intmax_t)(actual), (intmax_t)(expected))

#define test_not_equal(actual, expected) \
    test_not_equal_intmax((intmax_t)(actual), (intmax_t)(expected))

#define test_status_ok(expr) test_equal((expr), 0)

typedef int (*test_fn)(void);

typedef struct test_case {
    const char *name;
    test_fn fn;
} test_case;

/* Return 0 when scalar values are equal, otherwise print both values and return 1. */
int test_equal_intmax(intmax_t actual, intmax_t expected);

/* Return 0 when scalar values differ, otherwise print both values and return 1. */
int test_not_equal_intmax(intmax_t actual, intmax_t expected);

/* Return 0 when pointers are equal, otherwise print both addresses and return 1. */
int test_equal_ptr(const void *actual, const void *expected);

/* Return 0 when pointers differ, otherwise print both addresses and return 1. */
int test_not_equal_ptr(const void *actual, const void *expected);

/* Return 0 when ptr is NULL, otherwise print the address and return 1. */
int test_null(const void *ptr);

/* Return 0 when ptr is not NULL, otherwise print a failure and return 1. */
int test_not_null(const void *ptr);

/* Return 0 when C strings are equal, otherwise print both values and return 1. */
int test_equal_str(const char *actual, const char *expected);

/* Return whether two C strings are equal, treating two NULL pointers as equal. */
bool test_str_eq(const char *actual, const char *expected);

/* Run named test cases and return 1 if any case fails. */
int test_run(const test_case *cases, size_t count);

#endif
