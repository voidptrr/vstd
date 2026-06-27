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

#include "vstd/ds/hashset.h"
#include "vstd/ds/iterator.h"
#include "vstd/error.h"
#include "vstd/memory/test_allocator.h"
#include "vstd/testing.h"

static size_t custom_eq_calls;

static bool custom_u64_eq(const void *lhs, const void *rhs) {
    custom_eq_calls += 1;
    return *(const uint64_t *)lhs == *(const uint64_t *)rhs;
}

TEST(allocator) {
    test_allocator test_allocator;
    uint64_t value = 11;

    allocator *allocator = test_allocator_init(&test_allocator);
    hashset *set = NULL;
    if (test_equal(hashset_create(sizeof(uint64_t), NULL, allocator, &set), STATUS_OK)) {
        return 1;
    }
    test_allocator_reset_counts(&test_allocator);

    if (test_status_ok(hashset_insert(set, &value))) {

        return 1;
    }
    if (test_allocator.alloc_count != 1) {
        return 1;
    }

    hashset_destroy(set);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(contains) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    hashset *set = NULL;
    if (test_equal(hashset_create(sizeof(uint64_t), NULL, allocator, &set), STATUS_OK)) {
        return 1;
    }

    uint64_t present = 42;
    uint64_t missing = 7;
    if (test_equal(!hashset_contains(set, &present), true) != 0) {
        return 1;
    }

    if (test_status_ok(hashset_insert(set, &present))) {

        return 1;
    }
    if (test_equal(hashset_contains(set, &present), true) != 0) {
        return 1;
    }

    if (test_equal(!hashset_contains(set, &missing), true) != 0) {
        return 1;
    }

    for (uint64_t i = 0; i < 256; i++) {
        if (test_status_ok(hashset_insert(set, &i))) {
            return 1;
        }
    }

    if (test_equal(hashset_contains(set, &missing), true) != 0) {
        return 1;
    }

    hashset_destroy(set);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(get) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    hashset *set = NULL;
    if (test_equal(hashset_create(sizeof(uint64_t), NULL, allocator, &set), STATUS_OK)) {
        return 1;
    }
    uint64_t value = 42;
    uint64_t missing = 7;

    if (test_null(hashset_get(set, &value)) != 0) {
        return 1;
    }

    if (test_status_ok(hashset_insert(set, &value))) {

        return 1;
    }

    uint64_t *found = (uint64_t *)hashset_get(set, &value);
    if (test_not_null(found) != 0) {
        return 1;
    }
    if (test_equal(*found, value) != 0) {
        return 1;
    }

    const hashset *const_set = set;
    const uint64_t *const_found = (const uint64_t *)hashset_get_const(const_set, &value);
    if (test_not_null(const_found) != 0) {
        return 1;
    }
    if (test_equal(*const_found, value) != 0) {
        return 1;
    }

    if (test_null(hashset_get(set, &missing)) != 0) {
        return 1;
    }

    hashset_destroy(set);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(init) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    hashset *set = NULL;
    if (test_equal(hashset_create(sizeof(uint64_t), NULL, allocator, &set), STATUS_OK)) {
        return 1;
    }
    if (hashset_size(set) != 0) {
        return 1;
    }

    hashset_destroy(set);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(default_byte_equality) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    hashset *set = NULL;
    if (test_equal(hashset_create(sizeof(uint64_t), NULL, allocator, &set), STATUS_OK)) {
        return 1;
    }
    uint64_t value = 42;
    uint64_t same_value = 42;

    if (test_status_ok(hashset_insert(set, &value))) {

        return 1;
    }
    if (test_status_ok(hashset_insert(set, &same_value))) {
        return 1;
    }
    if (test_equal(hashset_contains(set, &same_value), true) != 0) {
        return 1;
    }
    if (hashset_size(set) != 1) {
        return 1;
    }

    hashset_destroy(set);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(custom_equality) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    uint64_t value = 42;
    uint64_t same_value = 42;

    custom_eq_calls = 0;
    hashset *set = NULL;
    if (test_equal(hashset_create(sizeof(uint64_t), custom_u64_eq, allocator, &set), STATUS_OK)) {
        return 1;
    }

    if (test_status_ok(hashset_insert(set, &value))) {

        return 1;
    }
    if (test_status_ok(hashset_insert(set, &same_value))) {
        return 1;
    }
    if (test_equal(hashset_contains(set, &same_value), true) != 0) {
        return 1;
    }
    if (hashset_size(set) != 1) {
        return 1;
    }
    if (test_equal(custom_eq_calls > 0, true) != 0) {
        return 1;
    }

    hashset_destroy(set);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(insert) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    hashset *set = NULL;
    if (test_equal(hashset_create(sizeof(uint64_t), NULL, allocator, &set), STATUS_OK)) {
        return 1;
    }

    uint64_t first = 42;
    if (test_status_ok(hashset_insert(set, &first))) {
        return 1;
    }
    if (test_status_ok(hashset_insert(set, &first))) {
        return 1;
    }
    if (hashset_size(set) != 1) {
        return 1;
    }

    for (uint64_t i = 0; i < 256; i++) {
        if (test_status_ok(hashset_insert(set, &i))) {
            return 1;
        }
    }

    if (hashset_size(set) != 256) {
        return 1;
    }

    hashset_destroy(set);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(reserve) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    hashset *set = NULL;
    if (test_equal(hashset_create(sizeof(uint64_t), NULL, allocator, &set), STATUS_OK)) {
        return 1;
    }
    if (test_status_ok(hashset_reserve(set, 512))) {
        return 1;
    }

    for (uint64_t i = 0; i < 512; i++) {
        if (test_status_ok(hashset_insert(set, &i))) {
            return 1;
        }
    }

    for (uint64_t i = 0; i < 512; i++) {
        if (test_equal(hashset_contains(set, &i), true) != 0) {
            return 1;
        }
    }

    hashset_destroy(set);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(remove) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    hashset *set = NULL;
    if (test_equal(hashset_create(sizeof(uint64_t), NULL, allocator, &set), STATUS_OK)) {
        return 1;
    }

    for (uint64_t i = 0; i < 256; i++) {
        if (test_status_ok(hashset_insert(set, &i))) {
            return 1;
        }
    }

    uint64_t removed = 42;
    hashset_remove(set, &removed);
    if (test_equal(!hashset_contains(set, &removed), true) != 0) {
        return 1;
    }
    if (hashset_size(set) != 255) {
        return 1;
    }

    hashset_remove(set, &removed);
    if (hashset_size(set) != 255) {
        return 1;
    }

    uint64_t first = 0;
    uint64_t last = 255;
    hashset_remove(set, &first);
    hashset_remove(set, &last);
    if (test_equal(!hashset_contains(set, &first), true) != 0) {
        return 1;
    }
    if (test_equal(!hashset_contains(set, &last), true) != 0) {
        return 1;
    }
    if (hashset_size(set) != 253) {
        return 1;
    }

    hashset_destroy(set);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(foreach_macro_walks_elements) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    hashset *set = NULL;
    if (test_equal(hashset_create(sizeof(uint64_t), NULL, allocator, &set), STATUS_OK)) {
        return 1;
    }
    const uint64_t *elem;
    uint64_t sum = 0;
    size_t count = 0;

    for (uint64_t i = 1; i <= 4; i++) {
        if (test_status_ok(hashset_insert(set, &i))) {
            return 1;
        }
    }

    hashset_for_each(uint64_t, elem, set) {
        sum += *elem;
        count += 1;
    }

    if (count != 4) {
        return 1;
    }
    if (sum != 10) {
        return 1;
    }

    hashset_destroy(set);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(iterator_walks_elements) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    hashset *set = NULL;
    if (test_equal(hashset_create(sizeof(uint64_t), NULL, allocator, &set), STATUS_OK)) {
        return 1;
    }
    const uint64_t *elem;
    uint64_t sum = 0;
    size_t count = 0;

    for (uint64_t i = 1; i <= 4; i++) {
        if (test_status_ok(hashset_insert(set, &i))) {
            return 1;
        }
    }

    iterator iter = hashset_get_iterator(set);
    while ((elem = (const uint64_t *)iterator_next(&iter)) != NULL) {
        sum += *elem;
        count += 1;
    }
    if (count != 4) {
        return 1;
    }
    if (sum != 10) {
        return 1;
    }

    hashset_destroy(set);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST_MAIN(
    TEST_CASE(allocator),
    TEST_CASE(contains),
    TEST_CASE(get),
    TEST_CASE(init),
    TEST_CASE(default_byte_equality),
    TEST_CASE(custom_equality),
    TEST_CASE(insert),
    TEST_CASE(reserve),
    TEST_CASE(remove),
    TEST_CASE(foreach_macro_walks_elements),
    TEST_CASE(iterator_walks_elements)
)
