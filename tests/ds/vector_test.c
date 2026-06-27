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

#include "vstd/ds/iterator.h"
#include "vstd/ds/vector.h"
#include "vstd/error.h"
#include "vstd/memory/test_allocator.h"
#include "vstd/testing.h"

static int cmp_int(const void *lhs, const void *rhs) {
    int a = *(const int *)lhs;
    int b = *(const int *)rhs;

    return (a > b) - (a < b);
}

static void int_to_double(void *context, const void *src, void *dst) {
    (void)context;

    *(double *)dst = (double)*(const int *)src;
}

typedef struct paged_array_iterator {
    const int *items;
    size_t size;
    size_t index;
    size_t page_size;
    size_t page_remaining;
    size_t pages_loaded;
} paged_array_iterator;

static const void *paged_array_next(void *context) {
    paged_array_iterator *iter = context;

    if (iter->index >= iter->size) {
        return NULL;
    }
    if (iter->page_remaining == 0) {
        iter->page_remaining = iter->page_size;
        iter->pages_loaded += 1;
    }

    iter->index += 1;
    iter->page_remaining -= 1;
    return &iter->items[iter->index - 1];
}

static iterator paged_array_iter(paged_array_iterator *state, const int *items, size_t size) {
    state->items = items;
    state->size = size;
    state->index = 0;
    state->page_size = 10;
    state->page_remaining = 0;
    state->pages_loaded = 0;
    return iterator_from_callback(state, paged_array_next);
}

TEST(init) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    vector *v = NULL;
    if (test_equal(vector_create(sizeof(int), allocator, &v), STATUS_OK)) {
        return 1;
    }
    int value = 1;

    if (vector_size(v) != 0) {
        return 1;
    }

    if (test_status_ok(vector_push(v, &value))) {

        return 1;
    }
    if (vector_size(v) != 1) {
        return 1;
    }

    vector_destroy(v);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(pop) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    vector *v = NULL;
    if (test_equal(vector_create(sizeof(int), allocator, &v), STATUS_OK)) {
        return 1;
    }
    int first = 7;
    int second = 11;

    if (test_null(vector_pop(v)) != 0) {
        return 1;
    }

    if (test_status_ok(vector_push(v, &first))) {

        return 1;
    }
    if (test_status_ok(vector_push(v, &second))) {
        return 1;
    }

    int *popped = (int *)vector_pop(v);
    if (test_not_null(popped) != 0) {
        return 1;
    }
    if (test_equal(*popped, second) != 0) {
        return 1;
    }

    popped = (int *)vector_pop(v);
    if (test_not_null(popped) != 0) {
        return 1;
    }
    if (test_equal(*popped, first) != 0) {
        return 1;
    }

    vector_destroy(v);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(push_single_element) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    vector *v = NULL;
    if (test_equal(vector_create(sizeof(int), allocator, &v), STATUS_OK)) {
        return 1;
    }
    int value = 42;

    if (test_status_ok(vector_push(v, &value))) {

        return 1;
    }

    if (vector_size(v) != 1) {
        return 1;
    }
    if (test_equal(*(int *)vector_get(v, 0), value) != 0) {
        return 1;
    }

    vector_destroy(v);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(push_grows_storage) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    vector *v = NULL;
    if (test_equal(vector_create(sizeof(int), allocator, &v), STATUS_OK)) {
        return 1;
    }

    for (size_t i = 0; i < 17; i++) {
        int value = (int)i;
        if (test_status_ok(vector_push(v, &value))) {
            return 1;
        }
    }

    if (vector_size(v) != 17) {
        return 1;
    }

    vector_destroy(v);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(push_preserves_existing_items_after_growth) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    vector *v = NULL;
    if (test_equal(vector_create(sizeof(int), allocator, &v), STATUS_OK)) {
        return 1;
    }

    for (size_t i = 0; i < 17; i++) {
        int value = (int)i;
        if (test_status_ok(vector_push(v, &value))) {
            return 1;
        }
    }

    for (size_t i = 0; i < 17; i++) {
        if (test_equal(*(int *)vector_get(v, i), (int)i) != 0) {
            return 1;
        }
    }

    vector_destroy(v);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(reserve_and_data_access) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    vector *v = NULL;
    if (test_equal(vector_create_with_capacity(sizeof(int), 2, allocator, &v), STATUS_OK)) {
        return 1;
    }
    if (vector_capacity(v) != 2) {
        return 1;
    }

    if (test_status_ok(vector_reserve(v, 32))) {

        return 1;
    }
    if (vector_capacity(v) < 32) {
        return 1;
    }

    int first = 7;
    int second = 11;
    if (test_status_ok(vector_push(v, &first))) {
        return 1;
    }
    if (test_status_ok(vector_push(v, &second))) {
        return 1;
    }

    const int *items = (const int *)vector_data_const(v);
    if (test_equal(items[0], 7) != 0) {
        return 1;
    }
    if (test_equal(items[1], 11) != 0) {
        return 1;
    }
    if (test_not_null(vector_data(v)) != 0) {
        return 1;
    }

    vector_destroy(v);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(iterator_walks_vector) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    vector *v = NULL;
    if (test_equal(vector_create(sizeof(int), allocator, &v), STATUS_OK)) {
        return 1;
    }
    const int *item;
    int expected = 0;

    for (int i = 0; i < 4; i++) {
        if (test_status_ok(vector_push(v, &i))) {
            return 1;
        }
    }

    iterator iter = vector_get_iterator(v);

    while ((item = iterator_next(&iter)) != NULL) {
        if (test_equal(*item, expected) != 0) {
            return 1;
        }
        expected += 1;
    }
    if (test_equal(expected, 4) != 0) {
        return 1;
    }

    vector_destroy(v);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(vector_for_each_macro_walks_items) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    vector *v = NULL;
    if (test_equal(vector_create(sizeof(int), allocator, &v), STATUS_OK)) {
        return 1;
    }
    int sum = 0;
    size_t count = 0;

    for (int i = 1; i <= 4; i++) {
        int value = i;
        if (test_status_ok(vector_push(v, &value))) {
            return 1;
        }
    }

    vector_for_each(int, item, v) {
        sum += *item;
        count += 1;
    }

    if (test_equal(sum, 10) != 0) {
        return 1;
    }
    if (count != 4) {
        return 1;
    }

    vector_destroy(v);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(custom_callback_iterator_takes_ten_at_a_time) {
    int values[25];
    paged_array_iterator state;
    const int *item;
    int expected = 0;
    size_t count = 0;

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        values[i] = (int)i;
    }

    iterator iter = paged_array_iter(&state, values, sizeof(values) / sizeof(values[0]));

    while ((item = iterator_next(&iter)) != NULL) {
        if (test_equal(*item, expected) != 0) {
            return 1;
        }
        expected += 1;
        count += 1;
    }

    if (count != 25) {
        return 1;
    }
    if (state.page_remaining != 5) {
        return 1;
    }
    if (state.pages_loaded != 3) {
        return 1;
    }

    return 0;
}

TEST(iterator_collect_copies_items) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    vector *v = NULL;
    if (test_equal(vector_create(sizeof(int), allocator, &v), STATUS_OK)) {
        return 1;
    }
    int expected[] = {1, 2, 3};

    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        if (test_status_ok(vector_push(v, &expected[i]))) {
            return 1;
        }
    }

    iterator iter = vector_get_iterator(v);
    vector *out = NULL;
    if (test_equal(iterator_collect(&iter, sizeof(int), allocator, &out), STATUS_OK)) {
        return 1;
    }
    vector_destroy(v);

    if (vector_size(out) != sizeof(expected) / sizeof(expected[0])) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        if (test_equal(*(int *)vector_get(out, i), expected[i]) != 0) {
            return 1;
        }
    }

    vector_destroy(out);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(iterator_collect_reserves_from_size_hint) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    vector *v = NULL;
    if (test_equal(vector_create(sizeof(int), allocator, &v), STATUS_OK)) {
        return 1;
    }

    for (int i = 0; i < 25; i++) {
        if (test_status_ok(vector_push(v, &i))) {
            return 1;
        }
    }

    iterator iter = vector_get_iterator(v);
    vector *out = NULL;
    if (test_equal(iterator_collect(&iter, sizeof(int), allocator, &out), STATUS_OK)) {
        return 1;
    }

    if (vector_size(out) != 25) {
        return 1;
    }
    if (vector_capacity(out) < 25) {
        return 1;
    }

    vector_destroy(out);
    vector_destroy(v);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(iterator_collect_map_changes_type) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    vector *v = NULL;
    if (test_equal(vector_create(sizeof(int), allocator, &v), STATUS_OK)) {
        return 1;
    }
    int values[] = {1, 2, 3};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        if (test_status_ok(vector_push(v, &values[i]))) {
            return 1;
        }
    }

    iterator iter = vector_get_iterator(v);
    vector *out = NULL;
    if (test_equal(
            iterator_collect_map(&iter, sizeof(double), int_to_double, NULL, allocator, &out),
            STATUS_OK
        )) {
        return 1;
    }
    vector_destroy(v);

    if (vector_size(out) != sizeof(values) / sizeof(values[0])) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        const double *value = (const double *)vector_get_const(out, i);
        if (test_equal((int)*value, values[i]) != 0) {
            return 1;
        }
    }

    vector_destroy(out);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(binary_search_bounds) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    vector *v = NULL;
    if (test_equal(vector_create(sizeof(int), allocator, &v), STATUS_OK)) {
        return 1;
    }
    int values[] = {1, 2, 2, 2, 4, 8};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        if (test_status_ok(vector_push(v, &values[i]))) {
            return 1;
        }
    }

    int key = 2;
    if (vector_lower_bound(v, &key, cmp_int) != 1) {
        return 1;
    }
    if (vector_upper_bound(v, &key, cmp_int) != 4) {
        return 1;
    }
    if (vector_binary_search(v, &key, cmp_int) != 1) {
        return 1;
    }

    key = 3;
    if (vector_lower_bound(v, &key, cmp_int) != 4) {
        return 1;
    }
    if (vector_upper_bound(v, &key, cmp_int) != 4) {
        return 1;
    }
    if (vector_binary_search(v, &key, cmp_int) != vector_size(v)) {
        return 1;
    }

    key = 0;
    if (vector_lower_bound(v, &key, cmp_int) != 0) {
        return 1;
    }
    key = 9;
    if (vector_upper_bound(v, &key, cmp_int) != vector_size(v)) {
        return 1;
    }

    vector_destroy(v);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST_MAIN(
    TEST_CASE(init),
    TEST_CASE(pop),
    TEST_CASE(push_single_element),
    TEST_CASE(push_grows_storage),
    TEST_CASE(push_preserves_existing_items_after_growth),
    TEST_CASE(reserve_and_data_access),
    TEST_CASE(iterator_walks_vector),
    TEST_CASE(vector_for_each_macro_walks_items),
    TEST_CASE(custom_callback_iterator_takes_ten_at_a_time),
    TEST_CASE(iterator_collect_copies_items),
    TEST_CASE(iterator_collect_reserves_from_size_hint),
    TEST_CASE(iterator_collect_map_changes_type),
    TEST_CASE(binary_search_bounds)
)
