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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "k4c/testing.h"

int k4c_test_equal_intmax(intmax_t actual, intmax_t expected) {
    if (actual == expected) {
        return 0;
    }

    fprintf(stderr, "test assertion failed: actual %jd, expected %jd\n", actual, expected);
    return 1;
}

int k4c_test_not_equal_intmax(intmax_t actual, intmax_t expected) {
    if (actual != expected) {
        return 0;
    }

    fprintf(stderr, "test assertion failed: values are equal (%jd)\n", actual);
    return 1;
}

int k4c_test_equal_ptr(const void *actual, const void *expected) {
    if (actual == expected) {
        return 0;
    }

    fprintf(stderr, "test assertion failed: actual %p, expected %p\n", actual, expected);
    return 1;
}

int k4c_test_not_equal_ptr(const void *actual, const void *expected) {
    if (actual != expected) {
        return 0;
    }

    fprintf(stderr, "test assertion failed: pointers are equal (%p)\n", actual);
    return 1;
}

int k4c_test_null(const void *ptr) {
    if (ptr == NULL) {
        return 0;
    }

    fprintf(stderr, "test assertion failed: expected NULL, got %p\n", ptr);
    return 1;
}

int k4c_test_not_null(const void *ptr) {
    if (ptr != NULL) {
        return 0;
    }

    fprintf(stderr, "test assertion failed: expected non-NULL pointer\n");
    return 1;
}

int k4c_test_equal_str(const char *actual, const char *expected) {
    if ((actual == NULL || expected == NULL) ? actual == expected : strcmp(actual, expected) == 0) {
        return 0;
    }

    fprintf(
        stderr,
        "test assertion failed: actual \"%s\", expected \"%s\"\n",
        actual == NULL ? "(null)" : actual,
        expected == NULL ? "(null)" : expected
    );
    return 1;
}

bool k4c_test_str_eq(const char *actual, const char *expected) {
    if (actual == NULL || expected == NULL) {
        return actual == expected;
    }

    return strcmp(actual, expected) == 0;
}

int k4c_test_run(const k4c_test_case *cases, size_t count) {
    int failed = 0;

    for (size_t i = 0; i < count; i++) {
        int k4c_status = cases[i].fn();
        if (k4c_status != 0) {
            fprintf(stderr, "test failed: %s\n", cases[i].name);
            failed = 1;
        }
    }

    return failed;
}
