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
#include <string.h>

#include "vstd/testing.h"

int vs_test_fail(const char *file, int line, const char *condition, const char *message) {
    if (message == NULL) {
        fprintf(stderr, "%s:%d: assertion failed: %s\n", file, line, condition);
    } else {
        fprintf(stderr, "%s:%d: assertion failed: %s: %s\n", file, line, condition, message);
    }

    return 1;
}

int vs_test_fail_eq(
    const char *file,
    int line,
    const char *actual_expr,
    const char *expected_expr
) {
    fprintf(stderr, "%s:%d: assertion failed: %s == %s\n", file, line, actual_expr, expected_expr);
    return 1;
}

int vs_test_fail_ptr_null(const char *file, int line, const char *expr, const void *ptr) {
    fprintf(stderr, "%s:%d: assertion failed: %s == NULL (actual %p)\n", file, line, expr, ptr);
    return 1;
}

int vs_test_fail_ptr_eq(
    const char *file,
    int line,
    const char *actual_expr,
    const char *expected_expr,
    const void *actual,
    const void *expected
) {
    fprintf(
        stderr,
        "%s:%d: assertion failed: %s == %s (actual %p, expected %p)\n",
        file,
        line,
        actual_expr,
        expected_expr,
        actual,
        expected
    );
    return 1;
}

int vs_test_fail_str_eq(
    const char *file,
    int line,
    const char *actual_expr,
    const char *expected_expr,
    const char *actual,
    const char *expected
) {
    fprintf(
        stderr,
        "%s:%d: assertion failed: %s == %s (actual \"%s\", expected \"%s\")\n",
        file,
        line,
        actual_expr,
        expected_expr,
        actual == NULL ? "(null)" : actual,
        expected == NULL ? "(null)" : expected
    );
    return 1;
}

bool vs_test_str_eq(const char *actual, const char *expected) {
    if (actual == NULL || expected == NULL) {
        return actual == expected;
    }

    return strcmp(actual, expected) == 0;
}

int vs_test_run(const vs_test_case *cases, size_t count) {
    int failed = 0;

    for (size_t i = 0; i < count; i++) {
        int status = cases[i].fn();
        if (status != 0) {
            fprintf(stderr, "test failed: %s\n", cases[i].name);
            failed = 1;
        }
    }

    return failed;
}
