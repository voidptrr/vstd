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

#include "vstd/datastruct/hashmap.h"
#include "vstd/datastruct/iterator.h"
#include "vstd/memory/test_allocator.h"
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
    if (test_allocator.alloc_count != 1) {
        return 1;
    }

    vs_hashmap_destroy(map);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
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
    if (vs_hashmap_size(map) != 0) {
        return 1;
    }

    vs_hashmap_destroy(map);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
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
    if (vs_test_not_null(out) != 0) {
        return 1;
    }
    if (vs_test_equal(*out, value) != 0) {
        return 1;
    }

    vs_hashmap_destroy(map);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
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
    if (vs_test_not_null(out) != 0) {
        return 1;
    }
    if (vs_test_equal(*out, value) != 0) {
        return 1;
    }
    if (vs_test_equal(custom_eq_calls > 0, true) != 0) {
        return 1;
    }

    vs_hashmap_destroy(map);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
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

    if (vs_test_null(vs_hashmap_get(map, &key)) != 0) {
        return 1;
    }

    vs_hashmap_put(map, &key, &value);
    out = (uint64_t *)vs_hashmap_get(map, &key);
    if (vs_test_not_null(out) != 0) {
        return 1;
    }
    if (vs_test_equal(*out, value) != 0) {
        return 1;
    }

    const_map = map;
    out = (const uint64_t *)vs_hashmap_get_const(const_map, &key);
    if (vs_test_not_null(out) != 0) {
        return 1;
    }
    if (vs_test_equal(*out, value) != 0) {
        return 1;
    }

    vs_hashmap_put(map, &key, &value2);
    out = (uint64_t *)vs_hashmap_get(map, &key);
    if (vs_test_not_null(out) != 0) {
        return 1;
    }
    if (vs_test_equal(*out, value2) != 0) {
        return 1;
    }
    if (vs_hashmap_size(map) != 1) {
        return 1;
    }

    vs_hashmap_destroy(map);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(reserve) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashmap *map;

    map = vs_hashmap_create(
        sizeof(uint64_t),
        sizeof(uint64_t),
        NULL,
        vs_test_allocator_adapter(&test_allocator)
    );
    vs_hashmap_reserve(map, 512);

    for (uint64_t i = 0; i < 512; i++) {
        uint64_t value = i * 5;
        vs_hashmap_put(map, &i, &value);
    }

    for (uint64_t i = 0; i < 512; i++) {
        const uint64_t *out = (const uint64_t *)vs_hashmap_get(map, &i);
        if (vs_test_not_null(out) != 0) {
            return 1;
        }
        if (vs_test_equal(*out, i * 5) != 0) {
            return 1;
        }
    }

    vs_hashmap_destroy(map);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
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

    if (vs_hashmap_size(map) != 256) {
        return 1;
    }

    vs_hashmap_remove(map, &key);
    if (vs_test_null(vs_hashmap_get(map, &key)) != 0) {
        return 1;
    }

    vs_hashmap_destroy(map);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(foreach_macros_walk_items) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashmap *map;
    const vs_hashmap_entry_view *entry;
    const uint64_t *key;
    const uint64_t *value;
    uint64_t key_sum = 0;
    uint64_t value_sum = 0;
    size_t count = 0;

    map = vs_hashmap_create(
        sizeof(uint64_t),
        sizeof(uint64_t),
        NULL,
        vs_test_allocator_adapter(&test_allocator)
    );
    for (uint64_t i = 1; i <= 4; i++) {
        uint64_t stored = i * 10;
        vs_hashmap_put(map, &i, &stored);
    }

    VS_HASHMAP_FOR_EACH_ENTRY(entry, map) {
        key_sum += *(const uint64_t *)entry->key;
        count += 1;
    }
    VS_HASHMAP_FOR_EACH_KEY(uint64_t, key, map) {
        key_sum += *key;
    }
    VS_HASHMAP_FOR_EACH_VALUE(uint64_t, value, map) {
        value_sum += *value;
    }

    if (count != 4) {
        return 1;
    }
    if (key_sum != 20) {
        return 1;
    }
    if (value_sum != 100) {
        return 1;
    }

    vs_hashmap_destroy(map);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(iterator_walks_entries) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashmap *map;
    vs_iterator iter;
    const vs_hashmap_entry_view *entry;
    uint64_t key_sum = 0;
    uint64_t value_sum = 0;
    size_t count = 0;

    map = vs_hashmap_create(
        sizeof(uint64_t),
        sizeof(uint64_t),
        NULL,
        vs_test_allocator_adapter(&test_allocator)
    );

    for (uint64_t i = 1; i <= 4; i++) {
        uint64_t value = i * 10;
        vs_hashmap_put(map, &i, &value);
    }

    iter = vs_hashmap_get_iterator(map, VS_HASHMAP_ITERATOR_ENTRY);
    while ((entry = (const vs_hashmap_entry_view *)vs_iterator_next(&iter)) != NULL) {
        key_sum += *(const uint64_t *)entry->key;
        value_sum += *(const uint64_t *)entry->value;
        count += 1;
    }
    if (count != 4) {
        return 1;
    }
    if (key_sum != 10) {
        return 1;
    }
    if (value_sum != 100) {
        return 1;
    }

    vs_hashmap_destroy(map);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(key_iterator_walks_keys) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashmap *map;
    vs_iterator iter;
    const uint64_t *key;
    uint64_t key_sum = 0;
    size_t count = 0;

    map = vs_hashmap_create(
        sizeof(uint64_t),
        sizeof(uint64_t),
        NULL,
        vs_test_allocator_adapter(&test_allocator)
    );

    for (uint64_t i = 1; i <= 4; i++) {
        uint64_t value = i * 10;
        vs_hashmap_put(map, &i, &value);
    }

    iter = vs_hashmap_get_iterator(map, VS_HASHMAP_ITERATOR_KEY);
    while ((key = (const uint64_t *)vs_iterator_next(&iter)) != NULL) {
        key_sum += *key;
        count += 1;
    }
    if (count != 4) {
        return 1;
    }
    if (key_sum != 10) {
        return 1;
    }

    vs_hashmap_destroy(map);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(value_iterator_walks_values) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashmap *map;
    vs_iterator iter;
    const uint64_t *value;
    uint64_t value_sum = 0;
    size_t count = 0;

    map = vs_hashmap_create(
        sizeof(uint64_t),
        sizeof(uint64_t),
        NULL,
        vs_test_allocator_adapter(&test_allocator)
    );

    for (uint64_t i = 1; i <= 4; i++) {
        uint64_t stored = i * 10;
        vs_hashmap_put(map, &i, &stored);
    }

    iter = vs_hashmap_get_iterator(map, VS_HASHMAP_ITERATOR_VALUE);
    while ((value = (const uint64_t *)vs_iterator_next(&iter)) != NULL) {
        value_sum += *value;
        count += 1;
    }
    if (count != 4) {
        return 1;
    }
    if (value_sum != 100) {
        return 1;
    }

    vs_hashmap_destroy(map);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST_MAIN(
    VS_TEST_CASE(allocator),
    VS_TEST_CASE(init),
    VS_TEST_CASE(default_byte_equality),
    VS_TEST_CASE(custom_equality),
    VS_TEST_CASE(put_get),
    VS_TEST_CASE(reserve),
    VS_TEST_CASE(remove_growth),
    VS_TEST_CASE(foreach_macros_walk_items),
    VS_TEST_CASE(iterator_walks_entries),
    VS_TEST_CASE(key_iterator_walks_keys),
    VS_TEST_CASE(value_iterator_walks_values)
)
