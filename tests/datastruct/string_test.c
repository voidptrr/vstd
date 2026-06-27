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

#include <stddef.h>

#include "vstd/datastruct/iterator.h"
#include "vstd/datastruct/string.h"
#include "vstd/error.h"
#include "vstd/memory/test_allocator.h"
#include "vstd/testing.h"

VS_TEST(allocator) {
    vs_test_allocator test_allocator;

    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_string value = NULL;
    if (vs_test_equal(vs_string_create("abc", allocator, &value), VS_STATUS_OK)) {
        return 1;
    }
    if (test_allocator.alloc_count != 1) {
        return 1;
    }
    if (test_allocator.realloc_count != 0) {
        return 1;
    }
    if (test_allocator.dealloc_count != 0) {
        return 1;
    }

    if (vs_test_status_ok(vs_string_append(&value, "012345678901234567890123456789"))) {

        return 1;
    }
    if (test_allocator.realloc_count != 1) {
        return 1;
    }
    if (vs_string_len(value) != 33) {
        return 1;
    }
    if (vs_test_equal_str(value, "abc012345678901234567890123456789") != 0) {
        return 1;
    }

    vs_string_destroy(value);
    if (test_allocator.dealloc_count != 1) {
        return 1;
    }
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(append) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_string value = NULL;
    if (vs_test_equal(vs_string_create("hello", allocator, &value), VS_STATUS_OK)) {
        return 1;
    }

    if (vs_test_status_ok(vs_string_append(&value, ", "))) {

        return 1;
    }
    if (vs_test_status_ok(vs_string_append(&value, "world"))) {
        return 1;
    }
    if (vs_test_status_ok(vs_string_append(&value, ""))) {
        return 1;
    }
    if (vs_string_len(value) != 12) {
        return 1;
    }
    if (vs_test_equal_str(value, "hello, world") != 0) {
        return 1;
    }

    if (vs_test_status_ok(vs_string_append(&value, "012345678901234567890123456789"))) {

        return 1;
    }
    if (vs_string_len(value) != 42) {
        return 1;
    }
    if (vs_test_equal_str(value, "hello, world012345678901234567890123456789") != 0) {
        return 1;
    }

    vs_string_destroy(value);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(clear) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_string value = NULL;
    if (vs_test_equal(vs_string_create("hello", allocator, &value), VS_STATUS_OK)) {
        return 1;
    }

    vs_string_clear(value);
    if (vs_string_len(value) != 0) {
        return 1;
    }
    if (vs_test_equal_str(value, "") != 0) {
        return 1;
    }

    if (vs_test_status_ok(vs_string_append(&value, "again"))) {

        return 1;
    }
    if (vs_test_equal_str(value, "again") != 0) {
        return 1;
    }

    vs_string_destroy(value);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(init) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_string empty = NULL;
    if (vs_test_equal(vs_string_create(NULL, allocator, &empty), VS_STATUS_OK)) {
        return 1;
    }
    if (vs_test_not_null(empty) != 0) {
        return 1;
    }
    if (vs_string_len(empty) != 0) {
        return 1;
    }
    if (vs_test_equal_str(empty, "") != 0) {
        return 1;
    }
    vs_string_destroy(empty);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }

    vs_string value = NULL;
    if (vs_test_equal(vs_string_create("hello", allocator, &value), VS_STATUS_OK)) {
        return 1;
    }
    if (vs_test_not_null(value) != 0) {
        return 1;
    }
    if (vs_string_len(value) != 5) {
        return 1;
    }
    if (vs_test_equal_str(value, "hello") != 0) {
        return 1;
    }
    vs_string_destroy(value);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(prepend) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_string value = NULL;
    if (vs_test_equal(vs_string_create("world", allocator, &value), VS_STATUS_OK)) {
        return 1;
    }

    if (vs_test_status_ok(vs_string_prepend(&value, "hello "))) {

        return 1;
    }
    if (vs_test_equal_str(value, "hello world") != 0) {
        return 1;
    }

    if (vs_test_status_ok(vs_string_prepend(&value, ""))) {

        return 1;
    }
    if (vs_test_equal_str(value, "hello world") != 0) {
        return 1;
    }

    vs_string_destroy(value);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(search) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_string value = NULL;
    if (vs_test_equal(vs_string_create("hello world", allocator, &value), VS_STATUS_OK)) {
        return 1;
    }

    if (vs_test_equal(vs_string_contains(value, "lo wo"), true) != 0) {
        return 1;
    }

    if (vs_test_equal(!vs_string_contains(value, "missing"), true) != 0) {
        return 1;
    }

    if (vs_test_equal(vs_string_starts_with(value, "hello"), true) != 0) {
        return 1;
    }
    if (vs_test_equal(!vs_string_starts_with(value, "world"), true) != 0) {
        return 1;
    }

    if (vs_test_equal(vs_string_ends_with(value, "world"), true) != 0) {
        return 1;
    }
    if (vs_test_equal(!vs_string_ends_with(value, "hello"), true) != 0) {
        return 1;
    }

    if (vs_test_equal(vs_string_contains(value, ""), true) != 0) {
        return 1;
    }
    if (vs_test_equal(vs_string_starts_with(value, ""), true) != 0) {
        return 1;
    }
    if (vs_test_equal(vs_string_ends_with(value, ""), true) != 0) {
        return 1;
    }

    vs_string_destroy(value);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(iterator_walks_bytes) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_string value = NULL;
    if (vs_test_equal(vs_string_create("abc", allocator, &value), VS_STATUS_OK)) {
        return 1;
    }
    vs_iterator iter = vs_string_get_iterator(value);
    const char *out;
    const char *expected = "abc";
    size_t index = 0;

    while ((out = (const char *)vs_iterator_next(&iter)) != NULL) {
        if (index >= 3) {
            return 1;
        }
        if (vs_test_equal(*out, expected[index]) != 0) {
            return 1;
        }
        index += 1;
    }
    if (index != 3) {
        return 1;
    }

    vs_string_destroy(value);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST_MAIN(
    VS_TEST_CASE(allocator),
    VS_TEST_CASE(append),
    VS_TEST_CASE(clear),
    VS_TEST_CASE(init),
    VS_TEST_CASE(prepend),
    VS_TEST_CASE(search),
    VS_TEST_CASE(iterator_walks_bytes)
)
