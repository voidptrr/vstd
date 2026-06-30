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

#include "k4c/allocators/allocator.h"
#include "k4c/allocators/test_allocator.h"
#include "k4c/datastruct/iterator.h"
#include "k4c/datastruct/string.h"
#include "k4c/error.h"
#include "k4c/testing.h"

K4C_TEST(k4c_allocator) {
    k4c_test_allocator k4c_test_allocator;

    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_string value = NULL;
    if (k4c_test_equal(k4c_string_create("abc", k4c_allocator, &value), K4C_STATUS_OK)) {
        return 1;
    }
    if (k4c_test_allocator.alloc_count != 1) {
        return 1;
    }
    if (k4c_test_allocator.realloc_count != 0) {
        return 1;
    }
    if (k4c_test_allocator.dealloc_count != 0) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_string_append(&value, "012345678901234567890123456789"))) {

        return 1;
    }
    if (k4c_test_allocator.realloc_count != 1) {
        return 1;
    }
    if (k4c_string_len(value) != 33) {
        return 1;
    }
    if (k4c_test_equal_str(value, "abc012345678901234567890123456789") != 0) {
        return 1;
    }

    k4c_string_destroy(value);
    if (k4c_test_allocator.dealloc_count != 1) {
        return 1;
    }
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(append) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_string value = NULL;
    if (k4c_test_equal(k4c_string_create("hello", k4c_allocator, &value), K4C_STATUS_OK)) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_string_append(&value, ", "))) {

        return 1;
    }
    if (k4c_test_status_ok(k4c_string_append(&value, "world"))) {
        return 1;
    }
    if (k4c_test_status_ok(k4c_string_append(&value, ""))) {
        return 1;
    }
    if (k4c_string_len(value) != 12) {
        return 1;
    }
    if (k4c_test_equal_str(value, "hello, world") != 0) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_string_append(&value, "012345678901234567890123456789"))) {

        return 1;
    }
    if (k4c_string_len(value) != 42) {
        return 1;
    }
    if (k4c_test_equal_str(value, "hello, world012345678901234567890123456789") != 0) {
        return 1;
    }

    k4c_string_destroy(value);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(clear) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_string value = NULL;
    if (k4c_test_equal(k4c_string_create("hello", k4c_allocator, &value), K4C_STATUS_OK)) {
        return 1;
    }

    k4c_string_clear(value);
    if (k4c_string_len(value) != 0) {
        return 1;
    }
    if (k4c_test_equal_str(value, "") != 0) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_string_append(&value, "again"))) {

        return 1;
    }
    if (k4c_test_equal_str(value, "again") != 0) {
        return 1;
    }

    k4c_string_destroy(value);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(init) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_string empty = NULL;
    if (k4c_test_equal(k4c_string_create(NULL, k4c_allocator, &empty), K4C_STATUS_OK)) {
        return 1;
    }
    if (k4c_test_not_null(empty) != 0) {
        return 1;
    }
    if (k4c_string_len(empty) != 0) {
        return 1;
    }
    if (k4c_test_equal_str(empty, "") != 0) {
        return 1;
    }
    k4c_string_destroy(empty);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }

    k4c_string value = NULL;
    if (k4c_test_equal(k4c_string_create("hello", k4c_allocator, &value), K4C_STATUS_OK)) {
        return 1;
    }
    if (k4c_test_not_null(value) != 0) {
        return 1;
    }
    if (k4c_string_len(value) != 5) {
        return 1;
    }
    if (k4c_test_equal_str(value, "hello") != 0) {
        return 1;
    }
    k4c_string_destroy(value);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(prepend) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_string value = NULL;
    if (k4c_test_equal(k4c_string_create("world", k4c_allocator, &value), K4C_STATUS_OK)) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_string_prepend(&value, "hello "))) {

        return 1;
    }
    if (k4c_test_equal_str(value, "hello world") != 0) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_string_prepend(&value, ""))) {

        return 1;
    }
    if (k4c_test_equal_str(value, "hello world") != 0) {
        return 1;
    }

    k4c_string_destroy(value);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(search) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_string value = NULL;
    if (k4c_test_equal(k4c_string_create("hello world", k4c_allocator, &value), K4C_STATUS_OK)) {
        return 1;
    }

    if (k4c_test_equal(k4c_string_contains(value, "lo wo"), true) != 0) {
        return 1;
    }

    if (k4c_test_equal(!k4c_string_contains(value, "missing"), true) != 0) {
        return 1;
    }

    if (k4c_test_equal(k4c_string_starts_with(value, "hello"), true) != 0) {
        return 1;
    }
    if (k4c_test_equal(!k4c_string_starts_with(value, "world"), true) != 0) {
        return 1;
    }

    if (k4c_test_equal(k4c_string_ends_with(value, "world"), true) != 0) {
        return 1;
    }
    if (k4c_test_equal(!k4c_string_ends_with(value, "hello"), true) != 0) {
        return 1;
    }

    if (k4c_test_equal(k4c_string_contains(value, ""), true) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_string_starts_with(value, ""), true) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_string_ends_with(value, ""), true) != 0) {
        return 1;
    }

    k4c_string_destroy(value);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(iterator_walks_bytes) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_string value = NULL;
    if (k4c_test_equal(k4c_string_create("abc", k4c_allocator, &value), K4C_STATUS_OK)) {
        return 1;
    }
    k4c_iterator iter = k4c_string_get_iterator(value);
    const char *out;
    const char *expected = "abc";
    size_t index = 0;

    while ((out = (const char *)k4c_iterator_next(&iter)) != NULL) {
        if (index >= 3) {
            return 1;
        }
        if (k4c_test_equal(*out, expected[index]) != 0) {
            return 1;
        }
        index += 1;
    }
    if (index != 3) {
        return 1;
    }

    k4c_string_destroy(value);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST_MAIN(
    K4C_TEST_CASE(k4c_allocator),
    K4C_TEST_CASE(append),
    K4C_TEST_CASE(clear),
    K4C_TEST_CASE(init),
    K4C_TEST_CASE(prepend),
    K4C_TEST_CASE(search),
    K4C_TEST_CASE(iterator_walks_bytes)
)
