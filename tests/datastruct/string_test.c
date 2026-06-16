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

#include "vstd/datastruct/string.h"
#include "vstd/memory/allocators/test_allocator.h"
#include "vstd/testing.h"

VS_TEST(allocator) {
    vs_test_allocator test_allocator;

    vs_test_allocator_init(&test_allocator);
    vs_string value = vs_string_create("abc", vs_test_allocator_adapter(&test_allocator));
    VS_TEST_ASSERT_EQ(test_allocator.alloc_count, 1);
    VS_TEST_ASSERT_EQ(test_allocator.realloc_count, 0);
    VS_TEST_ASSERT_EQ(test_allocator.dealloc_count, 0);

    vs_string_append(&value, "012345678901234567890123456789");
    VS_TEST_ASSERT_EQ(test_allocator.realloc_count, 1);
    VS_TEST_ASSERT_EQ(vs_string_len(value), 33);
    VS_TEST_ASSERT_STR_EQ(value, "abc012345678901234567890123456789");

    vs_string_destroy(value);
    VS_TEST_ASSERT_EQ(test_allocator.dealloc_count, 1);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(append) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_string value = vs_string_create("hello", vs_test_allocator_adapter(&test_allocator));

    vs_string_append(&value, ", ");
    vs_string_append(&value, "world");
    vs_string_append(&value, "");
    VS_TEST_ASSERT_EQ(vs_string_len(value), 12);
    VS_TEST_ASSERT_STR_EQ(value, "hello, world");

    vs_string_append(&value, "012345678901234567890123456789");
    VS_TEST_ASSERT_EQ(vs_string_len(value), 42);
    VS_TEST_ASSERT_STR_EQ(value, "hello, world012345678901234567890123456789");

    vs_string_destroy(value);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(clear) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_string value = vs_string_create("hello", vs_test_allocator_adapter(&test_allocator));

    vs_string_clear(value);
    VS_TEST_ASSERT_EQ(vs_string_len(value), 0);
    VS_TEST_ASSERT_STR_EQ(value, "");

    vs_string_append(&value, "again");
    VS_TEST_ASSERT_STR_EQ(value, "again");

    vs_string_destroy(value);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(init) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_string empty = vs_string_create(NULL, vs_test_allocator_adapter(&test_allocator));
    VS_TEST_ASSERT_PTR_NOT_NULL(empty);
    VS_TEST_ASSERT_EQ(vs_string_len(empty), 0);
    VS_TEST_ASSERT_STR_EQ(empty, "");
    vs_string_destroy(empty);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));

    vs_string value = vs_string_create("hello", vs_test_allocator_adapter(&test_allocator));
    VS_TEST_ASSERT_PTR_NOT_NULL(value);
    VS_TEST_ASSERT_EQ(vs_string_len(value), 5);
    VS_TEST_ASSERT_STR_EQ(value, "hello");
    vs_string_destroy(value);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(prepend) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_string value = vs_string_create("world", vs_test_allocator_adapter(&test_allocator));

    vs_string_prepend(&value, "hello ");
    VS_TEST_ASSERT_STR_EQ(value, "hello world");

    vs_string_prepend(&value, "");
    VS_TEST_ASSERT_STR_EQ(value, "hello world");

    vs_string_destroy(value);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(search) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_string value = vs_string_create("hello world", vs_test_allocator_adapter(&test_allocator));

    VS_TEST_ASSERT(vs_string_contains(value, "lo wo"));

    VS_TEST_ASSERT(!vs_string_contains(value, "missing"));

    VS_TEST_ASSERT(vs_string_starts_with(value, "hello"));
    VS_TEST_ASSERT(!vs_string_starts_with(value, "world"));

    VS_TEST_ASSERT(vs_string_ends_with(value, "world"));
    VS_TEST_ASSERT(!vs_string_ends_with(value, "hello"));

    VS_TEST_ASSERT(vs_string_contains(value, ""));
    VS_TEST_ASSERT(vs_string_starts_with(value, ""));
    VS_TEST_ASSERT(vs_string_ends_with(value, ""));

    vs_string_destroy(value);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST_MAIN(
    VS_TEST_CASE(allocator),
    VS_TEST_CASE(append),
    VS_TEST_CASE(clear),
    VS_TEST_CASE(init),
    VS_TEST_CASE(prepend),
    VS_TEST_CASE(search)
)
