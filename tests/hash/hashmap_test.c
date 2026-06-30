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

#include "k4c/allocators/allocator.h"
#include "k4c/allocators/test_allocator.h"
#include "k4c/datastruct/iterator.h"
#include "k4c/error.h"
#include "k4c/hash/hashmap.h"
#include "k4c/testing.h"

static size_t custom_eq_calls;

static bool custom_u64_eq(const void *lhs, const void *rhs) {
    custom_eq_calls += 1;
    return *(const uint64_t *)lhs == *(const uint64_t *)rhs;
}

K4C_TEST(k4c_allocator) {
    k4c_test_allocator k4c_test_allocator;
    uint64_t key = 7;
    uint64_t value = 11;

    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashmap *map = NULL;
    if (k4c_test_equal(
            k4c_hashmap_create(sizeof(uint64_t), sizeof(uint64_t), NULL, k4c_allocator, &map),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    k4c_test_allocator_reset_counts(&k4c_test_allocator);

    if (k4c_test_status_ok(k4c_hashmap_put(map, &key, &value))) {

        return 1;
    }
    if (k4c_test_allocator.alloc_count != 1) {
        return 1;
    }

    k4c_hashmap_destroy(map);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(init) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashmap *map = NULL;
    if (k4c_test_equal(
            k4c_hashmap_create(sizeof(uint64_t), sizeof(uint64_t), NULL, k4c_allocator, &map),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    if (k4c_hashmap_size(map) != 0) {
        return 1;
    }

    k4c_hashmap_destroy(map);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(default_byte_equality) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashmap *map = NULL;
    if (k4c_test_equal(
            k4c_hashmap_create(sizeof(uint64_t), sizeof(uint64_t), NULL, k4c_allocator, &map),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    uint64_t key = 7;
    uint64_t same_key = 7;
    uint64_t value = 11;

    if (k4c_test_status_ok(k4c_hashmap_put(map, &key, &value))) {

        return 1;
    }
    const uint64_t *out = (const uint64_t *)k4c_hashmap_get(map, &same_key);
    if (k4c_test_not_null(out) != 0) {
        return 1;
    }
    if (k4c_test_equal(*out, value) != 0) {
        return 1;
    }

    k4c_hashmap_destroy(map);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(custom_equality) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    uint64_t key = 7;
    uint64_t same_key = 7;
    uint64_t value = 11;

    custom_eq_calls = 0;
    k4c_hashmap *map = NULL;
    if (k4c_test_equal(
            k4c_hashmap_create(
                sizeof(uint64_t),
                sizeof(uint64_t),
                custom_u64_eq,
                k4c_allocator,
                &map
            ),
            K4C_STATUS_OK
        )) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_hashmap_put(map, &key, &value))) {

        return 1;
    }
    const uint64_t *out = (const uint64_t *)k4c_hashmap_get(map, &same_key);
    if (k4c_test_not_null(out) != 0) {
        return 1;
    }
    if (k4c_test_equal(*out, value) != 0) {
        return 1;
    }
    if (k4c_test_equal(custom_eq_calls > 0, true) != 0) {
        return 1;
    }

    k4c_hashmap_destroy(map);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(put_get) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashmap *map = NULL;
    if (k4c_test_equal(
            k4c_hashmap_create(sizeof(uint64_t), sizeof(uint64_t), NULL, k4c_allocator, &map),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    uint64_t key = 7;
    uint64_t value = 11;
    uint64_t value2 = 99;

    if (k4c_test_null(k4c_hashmap_get(map, &key)) != 0) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_hashmap_put(map, &key, &value))) {

        return 1;
    }
    const uint64_t *out = (const uint64_t *)k4c_hashmap_get(map, &key);
    if (k4c_test_not_null(out) != 0) {
        return 1;
    }
    if (k4c_test_equal(*out, value) != 0) {
        return 1;
    }

    const k4c_hashmap *const_map = map;
    out = (const uint64_t *)k4c_hashmap_get(const_map, &key);
    if (k4c_test_not_null(out) != 0) {
        return 1;
    }
    if (k4c_test_equal(*out, value) != 0) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_hashmap_put(map, &key, &value2))) {

        return 1;
    }
    out = (const uint64_t *)k4c_hashmap_get(map, &key);
    if (k4c_test_not_null(out) != 0) {
        return 1;
    }
    if (k4c_test_equal(*out, value2) != 0) {
        return 1;
    }
    if (k4c_hashmap_size(map) != 1) {
        return 1;
    }

    k4c_hashmap_destroy(map);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(reserve) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashmap *map = NULL;
    if (k4c_test_equal(
            k4c_hashmap_create(sizeof(uint64_t), sizeof(uint64_t), NULL, k4c_allocator, &map),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    if (k4c_test_status_ok(k4c_hashmap_reserve(map, 512))) {
        return 1;
    }

    for (uint64_t i = 0; i < 512; i++) {
        uint64_t value = i * 5;
        if (k4c_test_status_ok(k4c_hashmap_put(map, &i, &value))) {
            return 1;
        }
    }

    for (uint64_t i = 0; i < 512; i++) {
        const uint64_t *out = (const uint64_t *)k4c_hashmap_get(map, &i);
        if (k4c_test_not_null(out) != 0) {
            return 1;
        }
        if (k4c_test_equal(*out, i * 5) != 0) {
            return 1;
        }
    }

    k4c_hashmap_destroy(map);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(remove_growth) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashmap *map = NULL;
    if (k4c_test_equal(
            k4c_hashmap_create(sizeof(uint64_t), sizeof(uint64_t), NULL, k4c_allocator, &map),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    uint64_t key = 128;

    for (uint64_t i = 0; i < 256; i++) {
        uint64_t value = i * 3;
        if (k4c_test_status_ok(k4c_hashmap_put(map, &i, &value))) {
            return 1;
        }
    }

    if (k4c_hashmap_size(map) != 256) {
        return 1;
    }

    k4c_hashmap_remove(map, &key);
    if (k4c_test_null(k4c_hashmap_get(map, &key)) != 0) {
        return 1;
    }

    k4c_hashmap_destroy(map);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(foreach_macros_walk_items) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashmap *map = NULL;
    if (k4c_test_equal(
            k4c_hashmap_create(sizeof(uint64_t), sizeof(uint64_t), NULL, k4c_allocator, &map),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    const k4c_hashmap_entry_view *entry;
    const uint64_t *key;
    const uint64_t *value;
    uint64_t key_sum = 0;
    uint64_t value_sum = 0;
    size_t count = 0;

    for (uint64_t i = 1; i <= 4; i++) {
        uint64_t stored = i * 10;
        if (k4c_test_status_ok(k4c_hashmap_put(map, &i, &stored))) {
            return 1;
        }
    }

    k4c_hashmap_for_each_entry(entry, map) {
        key_sum += *(const uint64_t *)entry->key;
        count += 1;
    }
    k4c_hashmap_for_each_key(uint64_t, key, map) {
        key_sum += *key;
    }
    k4c_hashmap_for_each_value(uint64_t, value, map) {
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

    k4c_hashmap_destroy(map);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(iterator_walks_entries) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashmap *map = NULL;
    if (k4c_test_equal(
            k4c_hashmap_create(sizeof(uint64_t), sizeof(uint64_t), NULL, k4c_allocator, &map),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    const k4c_hashmap_entry_view *entry;
    uint64_t key_sum = 0;
    uint64_t value_sum = 0;
    size_t count = 0;

    for (uint64_t i = 1; i <= 4; i++) {
        uint64_t value = i * 10;
        if (k4c_test_status_ok(k4c_hashmap_put(map, &i, &value))) {
            return 1;
        }
    }

    k4c_iterator iter = k4c_hashmap_get_iterator(map, K4C_HASHMAP_ITERATOR_ENTRY);
    while ((entry = (const k4c_hashmap_entry_view *)k4c_iterator_next(&iter)) != NULL) {
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

    k4c_hashmap_destroy(map);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(key_iterator_walks_keys) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashmap *map = NULL;
    if (k4c_test_equal(
            k4c_hashmap_create(sizeof(uint64_t), sizeof(uint64_t), NULL, k4c_allocator, &map),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    const uint64_t *key;
    uint64_t key_sum = 0;
    size_t count = 0;

    for (uint64_t i = 1; i <= 4; i++) {
        uint64_t value = i * 10;
        if (k4c_test_status_ok(k4c_hashmap_put(map, &i, &value))) {
            return 1;
        }
    }

    k4c_iterator iter = k4c_hashmap_get_iterator(map, K4C_HASHMAP_ITERATOR_KEY);
    while ((key = (const uint64_t *)k4c_iterator_next(&iter)) != NULL) {
        key_sum += *key;
        count += 1;
    }
    if (count != 4) {
        return 1;
    }
    if (key_sum != 10) {
        return 1;
    }

    k4c_hashmap_destroy(map);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(value_iterator_walks_values) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashmap *map = NULL;
    if (k4c_test_equal(
            k4c_hashmap_create(sizeof(uint64_t), sizeof(uint64_t), NULL, k4c_allocator, &map),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    const uint64_t *value;
    uint64_t value_sum = 0;
    size_t count = 0;

    for (uint64_t i = 1; i <= 4; i++) {
        uint64_t stored = i * 10;
        if (k4c_test_status_ok(k4c_hashmap_put(map, &i, &stored))) {
            return 1;
        }
    }

    k4c_iterator iter = k4c_hashmap_get_iterator(map, K4C_HASHMAP_ITERATOR_VALUE);
    while ((value = (const uint64_t *)k4c_iterator_next(&iter)) != NULL) {
        value_sum += *value;
        count += 1;
    }
    if (count != 4) {
        return 1;
    }
    if (value_sum != 100) {
        return 1;
    }

    k4c_hashmap_destroy(map);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST_MAIN(
    K4C_TEST_CASE(k4c_allocator),
    K4C_TEST_CASE(init),
    K4C_TEST_CASE(default_byte_equality),
    K4C_TEST_CASE(custom_equality),
    K4C_TEST_CASE(put_get),
    K4C_TEST_CASE(reserve),
    K4C_TEST_CASE(remove_growth),
    K4C_TEST_CASE(foreach_macros_walk_items),
    K4C_TEST_CASE(iterator_walks_entries),
    K4C_TEST_CASE(key_iterator_walks_keys),
    K4C_TEST_CASE(value_iterator_walks_values)
)
