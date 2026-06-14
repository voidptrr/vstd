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
#include <stdint.h>

#include "ckit/testing.h"
#include "ckit/compare.h"

int main(void) {
    int32_t i32a = -2;
    int32_t i32b = 9;
    int32_t i32c = -2;

    CK_TEST_ASSERT(ck_cmp_i32(&i32a, &i32b) < 0);
    CK_TEST_ASSERT(ck_cmp_i32(&i32b, &i32a) > 0);
    CK_TEST_ASSERT_EQ(ck_cmp_i32(&i32a, &i32c), 0);

    uint64_t u64a = 1;
    uint64_t u64b = 2;
    uint64_t u64c = 2;

    CK_TEST_ASSERT(ck_cmp_u64(&u64a, &u64b) < 0);
    CK_TEST_ASSERT(ck_cmp_u64(&u64b, &u64a) > 0);
    CK_TEST_ASSERT_EQ(ck_cmp_u64(&u64b, &u64c), 0);

    size_t size_a = 1;
    size_t size_b = 2;
    size_t size_c = 2;

    CK_TEST_ASSERT(ck_cmp_size(&size_a, &size_b) < 0);
    CK_TEST_ASSERT(ck_cmp_size(&size_b, &size_a) > 0);
    CK_TEST_ASSERT_EQ(ck_cmp_size(&size_b, &size_c), 0);

    return 0;
}
