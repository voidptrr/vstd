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

#include "ckit/datastruct/string.h"
#include "ckit/testing.h"
#include "ckit/memory/allocators/test_allocator.h"

CK_TEST(allocator) {
    ck_test_allocator test_allocator;

    ck_test_allocator_init(&test_allocator);
    ck_string value = ck_string_create("abc", ck_test_allocator_adapter(&test_allocator));
    CK_TEST_ASSERT_EQ(test_allocator.alloc_count, 1);
    CK_TEST_ASSERT_EQ(test_allocator.realloc_count, 0);
    CK_TEST_ASSERT_EQ(test_allocator.dealloc_count, 0);

    ck_string_append(&value, "012345678901234567890123456789");
    CK_TEST_ASSERT_EQ(test_allocator.realloc_count, 1);
    CK_TEST_ASSERT_EQ(ck_string_len(value), 33);
    CK_TEST_ASSERT_STR_EQ(value, "abc012345678901234567890123456789");

    ck_string_destroy(value);
    CK_TEST_ASSERT_EQ(test_allocator.dealloc_count, 1);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(append) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_string value = ck_string_create("hello", ck_test_allocator_adapter(&test_allocator));

    ck_string_append(&value, ", ");
    ck_string_append(&value, "world");
    ck_string_append(&value, "");
    CK_TEST_ASSERT_EQ(ck_string_len(value), 12);
    CK_TEST_ASSERT_STR_EQ(value, "hello, world");

    ck_string_append(&value, "012345678901234567890123456789");
    CK_TEST_ASSERT_EQ(ck_string_len(value), 42);
    CK_TEST_ASSERT_STR_EQ(value, "hello, world012345678901234567890123456789");

    ck_string_destroy(value);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(clear) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_string value = ck_string_create("hello", ck_test_allocator_adapter(&test_allocator));

    ck_string_clear(value);
    CK_TEST_ASSERT_EQ(ck_string_len(value), 0);
    CK_TEST_ASSERT_STR_EQ(value, "");

    ck_string_append(&value, "again");
    CK_TEST_ASSERT_STR_EQ(value, "again");

    ck_string_destroy(value);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(init) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_string empty = ck_string_create(NULL, ck_test_allocator_adapter(&test_allocator));
    CK_TEST_ASSERT_PTR_NOT_NULL(empty);
    CK_TEST_ASSERT_EQ(ck_string_len(empty), 0);
    CK_TEST_ASSERT_STR_EQ(empty, "");
    ck_string_destroy(empty);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));

    ck_string value = ck_string_create("hello", ck_test_allocator_adapter(&test_allocator));
    CK_TEST_ASSERT_PTR_NOT_NULL(value);
    CK_TEST_ASSERT_EQ(ck_string_len(value), 5);
    CK_TEST_ASSERT_STR_EQ(value, "hello");
    ck_string_destroy(value);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(prepend) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_string value = ck_string_create("world", ck_test_allocator_adapter(&test_allocator));

    ck_string_prepend(&value, "hello ");
    CK_TEST_ASSERT_STR_EQ(value, "hello world");

    ck_string_prepend(&value, "");
    CK_TEST_ASSERT_STR_EQ(value, "hello world");

    ck_string_destroy(value);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(search) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_string value = ck_string_create("hello world", ck_test_allocator_adapter(&test_allocator));

    CK_TEST_ASSERT(ck_string_contains(value, "lo wo"));

    CK_TEST_ASSERT(!ck_string_contains(value, "missing"));

    CK_TEST_ASSERT(ck_string_starts_with(value, "hello"));
    CK_TEST_ASSERT(!ck_string_starts_with(value, "world"));

    CK_TEST_ASSERT(ck_string_ends_with(value, "world"));
    CK_TEST_ASSERT(!ck_string_ends_with(value, "hello"));

    CK_TEST_ASSERT(ck_string_contains(value, ""));
    CK_TEST_ASSERT(ck_string_starts_with(value, ""));
    CK_TEST_ASSERT(ck_string_ends_with(value, ""));

    ck_string_destroy(value);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST_MAIN(CK_TEST_CASE(allocator), CK_TEST_CASE(append), CK_TEST_CASE(clear), CK_TEST_CASE(init),
             CK_TEST_CASE(prepend), CK_TEST_CASE(search))
