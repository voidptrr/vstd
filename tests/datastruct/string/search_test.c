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
    ck_string value = ck_string_create("hello world", NULL);

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
    return 0;
}
