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

#include "ckit/testing.h"
#include "ckit/compare.h"
#include "ckit/datastruct/hashset.h"

int main(void) {
    ck_hashset *set;
    const ck_hashset *const_set;
    uint64_t value = 42;
    uint64_t missing = 7;
    uint64_t *found;
    const uint64_t *const_found;

    set = ck_hashset_create(sizeof(uint64_t), ck_eq_u64, NULL);

    CK_TEST_ASSERT_PTR_NULL(ck_hashset_get(set, &value));

    ck_hashset_insert(set, &value);

    found = (uint64_t *)ck_hashset_get(set, &value);
    CK_TEST_ASSERT_PTR_NOT_NULL(found);
    CK_TEST_ASSERT_EQ(*found, value);

    const_set = set;
    const_found = (const uint64_t *)ck_hashset_get_const(const_set, &value);
    CK_TEST_ASSERT_PTR_NOT_NULL(const_found);
    CK_TEST_ASSERT_EQ(*const_found, value);

    CK_TEST_ASSERT_PTR_NULL(ck_hashset_get(set, &missing));

    ck_hashset_destroy(set);
    return 0;
}
