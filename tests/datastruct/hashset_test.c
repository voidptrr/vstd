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

#include "vstd/compare.h"
#include "vstd/datastruct/hashset.h"
#include "vstd/memory/allocators/test_allocator.h"
#include "vstd/testing.h"

VS_TEST(allocator) {
    vs_test_allocator test_allocator;
    uint64_t value = 11;

    vs_test_allocator_init(&test_allocator);
    vs_hashset *set =
        vs_hashset_create(sizeof(uint64_t), vs_eq_u64, vs_test_allocator_adapter(&test_allocator));
    vs_test_allocator_reset_counts(&test_allocator);

    vs_hashset_insert(set, &value);
    VS_TEST_ASSERT_EQ(test_allocator.alloc_count, 1);

    vs_hashset_destroy(set);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(contains) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashset *set;

    set =
        vs_hashset_create(sizeof(uint64_t), vs_eq_u64, vs_test_allocator_adapter(&test_allocator));

    uint64_t present = 42;
    uint64_t missing = 7;
    VS_TEST_ASSERT(!vs_hashset_contains(set, &present));

    vs_hashset_insert(set, &present);
    VS_TEST_ASSERT(vs_hashset_contains(set, &present));

    VS_TEST_ASSERT(!vs_hashset_contains(set, &missing));

    for (uint64_t i = 0; i < 256; i++) {
        vs_hashset_insert(set, &i);
    }

    VS_TEST_ASSERT(vs_hashset_contains(set, &missing));

    vs_hashset_destroy(set);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(get) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashset *set;
    const vs_hashset *const_set;
    uint64_t value = 42;
    uint64_t missing = 7;
    uint64_t *found;
    const uint64_t *const_found;

    set =
        vs_hashset_create(sizeof(uint64_t), vs_eq_u64, vs_test_allocator_adapter(&test_allocator));

    VS_TEST_ASSERT_PTR_NULL(vs_hashset_get(set, &value));

    vs_hashset_insert(set, &value);

    found = (uint64_t *)vs_hashset_get(set, &value);
    VS_TEST_ASSERT_PTR_NOT_NULL(found);
    VS_TEST_ASSERT_EQ(*found, value);

    const_set = set;
    const_found = (const uint64_t *)vs_hashset_get_const(const_set, &value);
    VS_TEST_ASSERT_PTR_NOT_NULL(const_found);
    VS_TEST_ASSERT_EQ(*const_found, value);

    VS_TEST_ASSERT_PTR_NULL(vs_hashset_get(set, &missing));

    vs_hashset_destroy(set);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(init) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashset *set;

    set =
        vs_hashset_create(sizeof(uint64_t), vs_eq_u64, vs_test_allocator_adapter(&test_allocator));
    VS_TEST_ASSERT_EQ(vs_hashset_size(set), 0);

    vs_hashset_destroy(set);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(insert) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashset *set;

    set =
        vs_hashset_create(sizeof(uint64_t), vs_eq_u64, vs_test_allocator_adapter(&test_allocator));

    uint64_t first = 42;
    vs_hashset_insert(set, &first);
    vs_hashset_insert(set, &first);
    VS_TEST_ASSERT_EQ(vs_hashset_size(set), 1);

    for (uint64_t i = 0; i < 256; i++) {
        vs_hashset_insert(set, &i);
    }

    VS_TEST_ASSERT_EQ(vs_hashset_size(set), 256);

    vs_hashset_destroy(set);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(remove) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashset *set;

    set =
        vs_hashset_create(sizeof(uint64_t), vs_eq_u64, vs_test_allocator_adapter(&test_allocator));

    for (uint64_t i = 0; i < 256; i++) {
        vs_hashset_insert(set, &i);
    }

    uint64_t removed = 42;
    vs_hashset_remove(set, &removed);
    VS_TEST_ASSERT(!vs_hashset_contains(set, &removed));
    VS_TEST_ASSERT_EQ(vs_hashset_size(set), 255);

    vs_hashset_remove(set, &removed);
    VS_TEST_ASSERT_EQ(vs_hashset_size(set), 255);

    uint64_t first = 0;
    uint64_t last = 255;
    vs_hashset_remove(set, &first);
    vs_hashset_remove(set, &last);
    VS_TEST_ASSERT(!vs_hashset_contains(set, &first));
    VS_TEST_ASSERT(!vs_hashset_contains(set, &last));
    VS_TEST_ASSERT_EQ(vs_hashset_size(set), 253);

    vs_hashset_destroy(set);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST_MAIN(
    VS_TEST_CASE(allocator),
    VS_TEST_CASE(contains),
    VS_TEST_CASE(get),
    VS_TEST_CASE(init),
    VS_TEST_CASE(insert),
    VS_TEST_CASE(remove)
)
