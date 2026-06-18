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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "vstd/assert.h"
#include "vstd/datastruct/hashmap.h"
#include "vstd/memory/allocators/test_allocator.h"
#include "vstd/testing.h"

static size_t custom_eq_calls;

static bool custom_u64_eq(const void *lhs, const void *rhs) {
    custom_eq_calls += 1;
    return *(const uint64_t *)lhs == *(const uint64_t *)rhs;
}

VS_TEST(allocator) {
    vs_test_allocator test_allocator;
    uint64_t key = 7;
    uint64_t value = 11;

    vs_test_allocator_init(&test_allocator);
    vs_hashmap *map = vs_hashmap_create(
        sizeof(uint64_t),
        sizeof(uint64_t),
        NULL,
        vs_test_allocator_adapter(&test_allocator)
    );
    vs_test_allocator_reset_counts(&test_allocator);

    vs_hashmap_put(map, &key, &value);
    VS_ASSERT_EQ(test_allocator.alloc_count, 1);

    vs_hashmap_destroy(map);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(init) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashmap *map;

    map = vs_hashmap_create(
        sizeof(uint64_t),
        sizeof(uint64_t),
        NULL,
        vs_test_allocator_adapter(&test_allocator)
    );
    VS_ASSERT_EQ(vs_hashmap_size(map), 0);

    vs_hashmap_destroy(map);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(default_byte_equality) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashmap *map;
    uint64_t key = 7;
    uint64_t same_key = 7;
    uint64_t value = 11;
    const uint64_t *out;

    map = vs_hashmap_create(
        sizeof(uint64_t),
        sizeof(uint64_t),
        NULL,
        vs_test_allocator_adapter(&test_allocator)
    );

    vs_hashmap_put(map, &key, &value);
    out = (const uint64_t *)vs_hashmap_get(map, &same_key);
    VS_ASSERT_PTR_NOT_NULL(out);
    VS_ASSERT_EQ(*out, value);

    vs_hashmap_destroy(map);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(custom_equality) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashmap *map;
    uint64_t key = 7;
    uint64_t same_key = 7;
    uint64_t value = 11;
    const uint64_t *out;

    custom_eq_calls = 0;
    map = vs_hashmap_create(
        sizeof(uint64_t),
        sizeof(uint64_t),
        custom_u64_eq,
        vs_test_allocator_adapter(&test_allocator)
    );

    vs_hashmap_put(map, &key, &value);
    out = (const uint64_t *)vs_hashmap_get(map, &same_key);
    VS_ASSERT_PTR_NOT_NULL(out);
    VS_ASSERT_EQ(*out, value);
    VS_ASSERT(custom_eq_calls > 0);

    vs_hashmap_destroy(map);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(put_get) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashmap *map;
    uint64_t key = 7;
    uint64_t value = 11;
    uint64_t value2 = 99;
    const uint64_t *out;
    const vs_hashmap *const_map;

    map = vs_hashmap_create(
        sizeof(uint64_t),
        sizeof(uint64_t),
        NULL,
        vs_test_allocator_adapter(&test_allocator)
    );

    VS_ASSERT_PTR_NULL(vs_hashmap_get(map, &key));

    vs_hashmap_put(map, &key, &value);
    out = (uint64_t *)vs_hashmap_get(map, &key);
    VS_ASSERT_PTR_NOT_NULL(out);
    VS_ASSERT_EQ(*out, value);

    const_map = map;
    out = (const uint64_t *)vs_hashmap_get_const(const_map, &key);
    VS_ASSERT_PTR_NOT_NULL(out);
    VS_ASSERT_EQ(*out, value);

    vs_hashmap_put(map, &key, &value2);
    out = (uint64_t *)vs_hashmap_get(map, &key);
    VS_ASSERT_PTR_NOT_NULL(out);
    VS_ASSERT_EQ(*out, value2);
    VS_ASSERT_EQ(vs_hashmap_size(map), 1);

    vs_hashmap_destroy(map);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(remove_growth) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashmap *map;
    uint64_t key = 128;

    map = vs_hashmap_create(
        sizeof(uint64_t),
        sizeof(uint64_t),
        NULL,
        vs_test_allocator_adapter(&test_allocator)
    );

    for (uint64_t i = 0; i < 256; i++) {
        uint64_t value = i * 3;
        vs_hashmap_put(map, &i, &value);
    }

    VS_ASSERT_EQ(vs_hashmap_size(map), 256);

    vs_hashmap_remove(map, &key);
    VS_ASSERT_PTR_NULL(vs_hashmap_get(map, &key));

    vs_hashmap_destroy(map);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST_MAIN(
    VS_TEST_CASE(allocator),
    VS_TEST_CASE(init),
    VS_TEST_CASE(default_byte_equality),
    VS_TEST_CASE(custom_equality),
    VS_TEST_CASE(put_get),
    VS_TEST_CASE(remove_growth)
)
