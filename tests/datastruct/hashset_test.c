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
#include "ckit/compare.h"
#include "ckit/datastruct/hashset.h"
#include "ckit/testing.h"
#include "ckit/memory/allocators/test_allocator.h"

CK_TEST(allocator) {
    ck_test_allocator test_allocator;
    uint64_t value = 11;

    ck_test_allocator_init(&test_allocator);
    ck_hashset *set =
        ck_hashset_create(sizeof(uint64_t), ck_eq_u64, ck_test_allocator_adapter(&test_allocator));
    ck_test_allocator_reset_counts(&test_allocator);

    ck_hashset_insert(set, &value);
    CK_TEST_ASSERT_EQ(test_allocator.alloc_count, 1);

    ck_hashset_destroy(set);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(contains) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_hashset *set;

    set =
        ck_hashset_create(sizeof(uint64_t), ck_eq_u64, ck_test_allocator_adapter(&test_allocator));

    uint64_t present = 42;
    uint64_t missing = 7;
    CK_TEST_ASSERT(!ck_hashset_contains(set, &present));

    ck_hashset_insert(set, &present);
    CK_TEST_ASSERT(ck_hashset_contains(set, &present));

    CK_TEST_ASSERT(!ck_hashset_contains(set, &missing));

    for (uint64_t i = 0; i < 256; i++) {
        ck_hashset_insert(set, &i);
    }

    CK_TEST_ASSERT(ck_hashset_contains(set, &missing));

    ck_hashset_destroy(set);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(get) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_hashset *set;
    const ck_hashset *const_set;
    uint64_t value = 42;
    uint64_t missing = 7;
    uint64_t *found;
    const uint64_t *const_found;

    set =
        ck_hashset_create(sizeof(uint64_t), ck_eq_u64, ck_test_allocator_adapter(&test_allocator));

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
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(init) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_hashset *set;

    set =
        ck_hashset_create(sizeof(uint64_t), ck_eq_u64, ck_test_allocator_adapter(&test_allocator));
    CK_TEST_ASSERT_EQ(ck_hashset_size(set), 0);

    ck_hashset_destroy(set);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(insert) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_hashset *set;

    set =
        ck_hashset_create(sizeof(uint64_t), ck_eq_u64, ck_test_allocator_adapter(&test_allocator));

    uint64_t first = 42;
    ck_hashset_insert(set, &first);
    ck_hashset_insert(set, &first);
    CK_TEST_ASSERT_EQ(ck_hashset_size(set), 1);

    for (uint64_t i = 0; i < 256; i++) {
        ck_hashset_insert(set, &i);
    }

    CK_TEST_ASSERT_EQ(ck_hashset_size(set), 256);

    ck_hashset_destroy(set);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(remove) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_hashset *set;

    set =
        ck_hashset_create(sizeof(uint64_t), ck_eq_u64, ck_test_allocator_adapter(&test_allocator));

    for (uint64_t i = 0; i < 256; i++) {
        ck_hashset_insert(set, &i);
    }

    uint64_t removed = 42;
    ck_hashset_remove(set, &removed);
    CK_TEST_ASSERT(!ck_hashset_contains(set, &removed));
    CK_TEST_ASSERT_EQ(ck_hashset_size(set), 255);

    ck_hashset_remove(set, &removed);
    CK_TEST_ASSERT_EQ(ck_hashset_size(set), 255);

    uint64_t first = 0;
    uint64_t last = 255;
    ck_hashset_remove(set, &first);
    ck_hashset_remove(set, &last);
    CK_TEST_ASSERT(!ck_hashset_contains(set, &first));
    CK_TEST_ASSERT(!ck_hashset_contains(set, &last));
    CK_TEST_ASSERT_EQ(ck_hashset_size(set), 253);

    ck_hashset_destroy(set);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST_MAIN(CK_TEST_CASE(allocator), CK_TEST_CASE(contains), CK_TEST_CASE(get), CK_TEST_CASE(init),
             CK_TEST_CASE(insert), CK_TEST_CASE(remove))
