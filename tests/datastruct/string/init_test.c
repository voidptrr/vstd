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

#include "ckit/testing.h"
#include "ckit/datastruct/string.h"

int main(void) {
    ck_string empty = ck_string_create(NULL, NULL);
    CK_TEST_ASSERT_PTR_NOT_NULL(empty);
    CK_TEST_ASSERT_EQ(ck_string_len(empty), 0);
    CK_TEST_ASSERT_STR_EQ(empty, "");
    ck_string_destroy(empty);

    ck_string value = ck_string_create("hello", NULL);
    CK_TEST_ASSERT_PTR_NOT_NULL(value);
    CK_TEST_ASSERT_EQ(ck_string_len(value), 5);
    CK_TEST_ASSERT_STR_EQ(value, "hello");
    ck_string_destroy(value);
    return 0;
}
