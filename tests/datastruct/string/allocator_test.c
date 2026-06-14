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

int main(void) {
    ck_test_allocator test_allocator;

    ck_test_allocator_init(&test_allocator);
    ck_string value = ck_string_create("abc", ck_test_allocator_allocator(&test_allocator));
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
