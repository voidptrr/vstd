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
#include "k4c/hash/hashset.h"
#include "k4c/testing.h"

static size_t custom_eq_calls;

static bool custom_u64_eq(const void *lhs, const void *rhs) {
    custom_eq_calls += 1;
    return *(const uint64_t *)lhs == *(const uint64_t *)rhs;
}

K4C_TEST(k4c_allocator) {
    k4c_test_allocator k4c_test_allocator;
    uint64_t value = 11;

    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashset *set = NULL;
    if (k4c_test_equal(
            k4c_hashset_create(sizeof(uint64_t), NULL, k4c_allocator, &set),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    k4c_test_allocator_reset_counts(&k4c_test_allocator);

    if (k4c_test_status_ok(k4c_hashset_insert(set, &value))) {

        return 1;
    }
    if (k4c_test_allocator.alloc_count != 1) {
        return 1;
    }

    k4c_hashset_destroy(set);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(contains) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashset *set = NULL;
    if (k4c_test_equal(
            k4c_hashset_create(sizeof(uint64_t), NULL, k4c_allocator, &set),
            K4C_STATUS_OK
        )) {
        return 1;
    }

    uint64_t present = 42;
    uint64_t missing = 7;
    if (k4c_test_equal(!k4c_hashset_contains(set, &present), true) != 0) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_hashset_insert(set, &present))) {

        return 1;
    }
    if (k4c_test_equal(k4c_hashset_contains(set, &present), true) != 0) {
        return 1;
    }

    if (k4c_test_equal(!k4c_hashset_contains(set, &missing), true) != 0) {
        return 1;
    }

    for (uint64_t i = 0; i < 256; i++) {
        if (k4c_test_status_ok(k4c_hashset_insert(set, &i))) {
            return 1;
        }
    }

    if (k4c_test_equal(k4c_hashset_contains(set, &missing), true) != 0) {
        return 1;
    }

    k4c_hashset_destroy(set);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(get) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashset *set = NULL;
    if (k4c_test_equal(
            k4c_hashset_create(sizeof(uint64_t), NULL, k4c_allocator, &set),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    uint64_t value = 42;
    uint64_t missing = 7;

    if (k4c_test_null(k4c_hashset_get(set, &value)) != 0) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_hashset_insert(set, &value))) {

        return 1;
    }

    uint64_t *found = (uint64_t *)k4c_hashset_get(set, &value);
    if (k4c_test_not_null(found) != 0) {
        return 1;
    }
    if (k4c_test_equal(*found, value) != 0) {
        return 1;
    }

    const k4c_hashset *const_set = set;
    const uint64_t *const_found = (const uint64_t *)k4c_hashset_get(const_set, &value);
    if (k4c_test_not_null(const_found) != 0) {
        return 1;
    }
    if (k4c_test_equal(*const_found, value) != 0) {
        return 1;
    }

    if (k4c_test_null(k4c_hashset_get(set, &missing)) != 0) {
        return 1;
    }

    k4c_hashset_destroy(set);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(init) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashset *set = NULL;
    if (k4c_test_equal(
            k4c_hashset_create(sizeof(uint64_t), NULL, k4c_allocator, &set),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    if (k4c_hashset_size(set) != 0) {
        return 1;
    }

    k4c_hashset_destroy(set);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(default_byte_equality) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashset *set = NULL;
    if (k4c_test_equal(
            k4c_hashset_create(sizeof(uint64_t), NULL, k4c_allocator, &set),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    uint64_t value = 42;
    uint64_t same_value = 42;

    if (k4c_test_status_ok(k4c_hashset_insert(set, &value))) {

        return 1;
    }
    if (k4c_test_status_ok(k4c_hashset_insert(set, &same_value))) {
        return 1;
    }
    if (k4c_test_equal(k4c_hashset_contains(set, &same_value), true) != 0) {
        return 1;
    }
    if (k4c_hashset_size(set) != 1) {
        return 1;
    }

    k4c_hashset_destroy(set);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(custom_equality) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    uint64_t value = 42;
    uint64_t same_value = 42;

    custom_eq_calls = 0;
    k4c_hashset *set = NULL;
    if (k4c_test_equal(
            k4c_hashset_create(sizeof(uint64_t), custom_u64_eq, k4c_allocator, &set),
            K4C_STATUS_OK
        )) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_hashset_insert(set, &value))) {

        return 1;
    }
    if (k4c_test_status_ok(k4c_hashset_insert(set, &same_value))) {
        return 1;
    }
    if (k4c_test_equal(k4c_hashset_contains(set, &same_value), true) != 0) {
        return 1;
    }
    if (k4c_hashset_size(set) != 1) {
        return 1;
    }
    if (k4c_test_equal(custom_eq_calls > 0, true) != 0) {
        return 1;
    }

    k4c_hashset_destroy(set);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(insert) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashset *set = NULL;
    if (k4c_test_equal(
            k4c_hashset_create(sizeof(uint64_t), NULL, k4c_allocator, &set),
            K4C_STATUS_OK
        )) {
        return 1;
    }

    uint64_t first = 42;
    if (k4c_test_status_ok(k4c_hashset_insert(set, &first))) {
        return 1;
    }
    if (k4c_test_status_ok(k4c_hashset_insert(set, &first))) {
        return 1;
    }
    if (k4c_hashset_size(set) != 1) {
        return 1;
    }

    for (uint64_t i = 0; i < 256; i++) {
        if (k4c_test_status_ok(k4c_hashset_insert(set, &i))) {
            return 1;
        }
    }

    if (k4c_hashset_size(set) != 256) {
        return 1;
    }

    k4c_hashset_destroy(set);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(reserve) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashset *set = NULL;
    if (k4c_test_equal(
            k4c_hashset_create(sizeof(uint64_t), NULL, k4c_allocator, &set),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    if (k4c_test_status_ok(k4c_hashset_reserve(set, 512))) {
        return 1;
    }

    for (uint64_t i = 0; i < 512; i++) {
        if (k4c_test_status_ok(k4c_hashset_insert(set, &i))) {
            return 1;
        }
    }

    for (uint64_t i = 0; i < 512; i++) {
        if (k4c_test_equal(k4c_hashset_contains(set, &i), true) != 0) {
            return 1;
        }
    }

    k4c_hashset_destroy(set);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(remove) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashset *set = NULL;
    if (k4c_test_equal(
            k4c_hashset_create(sizeof(uint64_t), NULL, k4c_allocator, &set),
            K4C_STATUS_OK
        )) {
        return 1;
    }

    for (uint64_t i = 0; i < 256; i++) {
        if (k4c_test_status_ok(k4c_hashset_insert(set, &i))) {
            return 1;
        }
    }

    uint64_t removed = 42;
    k4c_hashset_remove(set, &removed);
    if (k4c_test_equal(!k4c_hashset_contains(set, &removed), true) != 0) {
        return 1;
    }
    if (k4c_hashset_size(set) != 255) {
        return 1;
    }

    k4c_hashset_remove(set, &removed);
    if (k4c_hashset_size(set) != 255) {
        return 1;
    }

    uint64_t first = 0;
    uint64_t last = 255;
    k4c_hashset_remove(set, &first);
    k4c_hashset_remove(set, &last);
    if (k4c_test_equal(!k4c_hashset_contains(set, &first), true) != 0) {
        return 1;
    }
    if (k4c_test_equal(!k4c_hashset_contains(set, &last), true) != 0) {
        return 1;
    }
    if (k4c_hashset_size(set) != 253) {
        return 1;
    }

    k4c_hashset_destroy(set);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(foreach_macro_walks_elements) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashset *set = NULL;
    if (k4c_test_equal(
            k4c_hashset_create(sizeof(uint64_t), NULL, k4c_allocator, &set),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    const uint64_t *elem;
    uint64_t sum = 0;
    size_t count = 0;

    for (uint64_t i = 1; i <= 4; i++) {
        if (k4c_test_status_ok(k4c_hashset_insert(set, &i))) {
            return 1;
        }
    }

    k4c_hashset_for_each(uint64_t, elem, set) {
        sum += *elem;
        count += 1;
    }

    if (count != 4) {
        return 1;
    }
    if (sum != 10) {
        return 1;
    }

    k4c_hashset_destroy(set);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(iterator_walks_elements) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_hashset *set = NULL;
    if (k4c_test_equal(
            k4c_hashset_create(sizeof(uint64_t), NULL, k4c_allocator, &set),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    const uint64_t *elem;
    uint64_t sum = 0;
    size_t count = 0;

    for (uint64_t i = 1; i <= 4; i++) {
        if (k4c_test_status_ok(k4c_hashset_insert(set, &i))) {
            return 1;
        }
    }

    k4c_iterator iter = k4c_hashset_get_iterator(set);
    while ((elem = (const uint64_t *)k4c_iterator_next(&iter)) != NULL) {
        sum += *elem;
        count += 1;
    }
    if (count != 4) {
        return 1;
    }
    if (sum != 10) {
        return 1;
    }

    k4c_hashset_destroy(set);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST_MAIN(
    K4C_TEST_CASE(k4c_allocator),
    K4C_TEST_CASE(contains),
    K4C_TEST_CASE(get),
    K4C_TEST_CASE(init),
    K4C_TEST_CASE(default_byte_equality),
    K4C_TEST_CASE(custom_equality),
    K4C_TEST_CASE(insert),
    K4C_TEST_CASE(reserve),
    K4C_TEST_CASE(remove),
    K4C_TEST_CASE(foreach_macro_walks_elements),
    K4C_TEST_CASE(iterator_walks_elements)
)
