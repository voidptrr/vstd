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

#include "k4c/allocators/allocator.h"
#include "k4c/allocators/test_allocator.h"
#include "k4c/datastruct/iterator.h"
#include "k4c/datastruct/vector.h"
#include "k4c/error.h"
#include "k4c/testing.h"

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

static k4c_iterator paged_array_iter(paged_array_iterator *state, const int *items, size_t size) {
    state->items = items;
    state->size = size;
    state->index = 0;
    state->page_size = 10;
    state->page_remaining = 0;
    state->pages_loaded = 0;
    return k4c_iterator_from_callback(state, paged_array_next);
}

K4C_TEST(init) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_vector *v = NULL;
    if (k4c_test_equal(k4c_vector_create(sizeof(int), k4c_allocator, &v), K4C_STATUS_OK)) {
        return 1;
    }
    int value = 1;

    if (k4c_vector_size(v) != 0) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_vector_push(v, &value))) {

        return 1;
    }
    if (k4c_vector_size(v) != 1) {
        return 1;
    }

    k4c_vector_destroy(v);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(pop) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_vector *v = NULL;
    if (k4c_test_equal(k4c_vector_create(sizeof(int), k4c_allocator, &v), K4C_STATUS_OK)) {
        return 1;
    }
    int first = 7;
    int second = 11;

    if (k4c_test_null(k4c_vector_pop(v)) != 0) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_vector_push(v, &first))) {

        return 1;
    }
    if (k4c_test_status_ok(k4c_vector_push(v, &second))) {
        return 1;
    }

    int *popped = (int *)k4c_vector_pop(v);
    if (k4c_test_not_null(popped) != 0) {
        return 1;
    }
    if (k4c_test_equal(*popped, second) != 0) {
        return 1;
    }

    popped = (int *)k4c_vector_pop(v);
    if (k4c_test_not_null(popped) != 0) {
        return 1;
    }
    if (k4c_test_equal(*popped, first) != 0) {
        return 1;
    }

    k4c_vector_destroy(v);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(push_single_element) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_vector *v = NULL;
    if (k4c_test_equal(k4c_vector_create(sizeof(int), k4c_allocator, &v), K4C_STATUS_OK)) {
        return 1;
    }
    int value = 42;

    if (k4c_test_status_ok(k4c_vector_push(v, &value))) {

        return 1;
    }

    if (k4c_vector_size(v) != 1) {
        return 1;
    }
    if (k4c_test_equal(*(int *)k4c_vector_get(v, 0), value) != 0) {
        return 1;
    }

    k4c_vector_destroy(v);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(push_grows_storage) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_vector *v = NULL;
    if (k4c_test_equal(k4c_vector_create(sizeof(int), k4c_allocator, &v), K4C_STATUS_OK)) {
        return 1;
    }

    for (size_t i = 0; i < 17; i++) {
        int value = (int)i;
        if (k4c_test_status_ok(k4c_vector_push(v, &value))) {
            return 1;
        }
    }

    if (k4c_vector_size(v) != 17) {
        return 1;
    }

    k4c_vector_destroy(v);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(push_preserves_existing_items_after_growth) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_vector *v = NULL;
    if (k4c_test_equal(k4c_vector_create(sizeof(int), k4c_allocator, &v), K4C_STATUS_OK)) {
        return 1;
    }

    for (size_t i = 0; i < 17; i++) {
        int value = (int)i;
        if (k4c_test_status_ok(k4c_vector_push(v, &value))) {
            return 1;
        }
    }

    for (size_t i = 0; i < 17; i++) {
        if (k4c_test_equal(*(int *)k4c_vector_get(v, i), (int)i) != 0) {
            return 1;
        }
    }

    k4c_vector_destroy(v);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(reserve_and_data_access) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_vector *v = NULL;
    if (k4c_test_equal(
            k4c_vector_create_with_capacity(sizeof(int), 2, k4c_allocator, &v),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    if (k4c_vector_capacity(v) != 2) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_vector_reserve(v, 32))) {

        return 1;
    }
    if (k4c_vector_capacity(v) < 32) {
        return 1;
    }

    int first = 7;
    int second = 11;
    if (k4c_test_status_ok(k4c_vector_push(v, &first))) {
        return 1;
    }
    if (k4c_test_status_ok(k4c_vector_push(v, &second))) {
        return 1;
    }

    const int *items = (const int *)k4c_vector_data(v);
    if (k4c_test_equal(items[0], 7) != 0) {
        return 1;
    }
    if (k4c_test_equal(items[1], 11) != 0) {
        return 1;
    }
    if (k4c_test_not_null(k4c_vector_data(v)) != 0) {
        return 1;
    }

    k4c_vector_destroy(v);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(iterator_walks_vector) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_vector *v = NULL;
    if (k4c_test_equal(k4c_vector_create(sizeof(int), k4c_allocator, &v), K4C_STATUS_OK)) {
        return 1;
    }
    const int *item;
    int expected = 0;

    for (int i = 0; i < 4; i++) {
        if (k4c_test_status_ok(k4c_vector_push(v, &i))) {
            return 1;
        }
    }

    k4c_iterator iter = k4c_vector_get_iterator(v);

    while ((item = k4c_iterator_next(&iter)) != NULL) {
        if (k4c_test_equal(*item, expected) != 0) {
            return 1;
        }
        expected += 1;
    }
    if (k4c_test_equal(expected, 4) != 0) {
        return 1;
    }

    k4c_vector_destroy(v);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(vector_for_each_macro_walks_items) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_vector *v = NULL;
    if (k4c_test_equal(k4c_vector_create(sizeof(int), k4c_allocator, &v), K4C_STATUS_OK)) {
        return 1;
    }
    int sum = 0;
    size_t count = 0;

    for (int i = 1; i <= 4; i++) {
        int value = i;
        if (k4c_test_status_ok(k4c_vector_push(v, &value))) {
            return 1;
        }
    }

    k4c_vector_for_each(int, item, v) {
        sum += *item;
        count += 1;
    }

    if (k4c_test_equal(sum, 10) != 0) {
        return 1;
    }
    if (count != 4) {
        return 1;
    }

    k4c_vector_destroy(v);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(custom_callback_iterator_takes_ten_at_a_time) {
    int values[25];
    paged_array_iterator state;
    const int *item;
    int expected = 0;
    size_t count = 0;

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        values[i] = (int)i;
    }

    k4c_iterator iter = paged_array_iter(&state, values, sizeof(values) / sizeof(values[0]));

    while ((item = k4c_iterator_next(&iter)) != NULL) {
        if (k4c_test_equal(*item, expected) != 0) {
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

K4C_TEST(vector_collect_copies_items) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_vector *v = NULL;
    if (k4c_test_equal(k4c_vector_create(sizeof(int), k4c_allocator, &v), K4C_STATUS_OK)) {
        return 1;
    }
    int expected[] = {1, 2, 3};

    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        if (k4c_test_status_ok(k4c_vector_push(v, &expected[i]))) {
            return 1;
        }
    }

    k4c_iterator iter = k4c_vector_get_iterator(v);
    k4c_vector *out = NULL;
    if (k4c_test_equal(
            k4c_vector_collect(&iter, sizeof(int), k4c_allocator, &out),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    k4c_vector_destroy(v);

    if (k4c_vector_size(out) != sizeof(expected) / sizeof(expected[0])) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        if (k4c_test_equal(*(int *)k4c_vector_get(out, i), expected[i]) != 0) {
            return 1;
        }
    }

    k4c_vector_destroy(out);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(vector_collect_reserves_from_size_hint) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_vector *v = NULL;
    if (k4c_test_equal(k4c_vector_create(sizeof(int), k4c_allocator, &v), K4C_STATUS_OK)) {
        return 1;
    }

    for (int i = 0; i < 25; i++) {
        if (k4c_test_status_ok(k4c_vector_push(v, &i))) {
            return 1;
        }
    }

    k4c_iterator iter = k4c_vector_get_iterator(v);
    k4c_vector *out = NULL;
    if (k4c_test_equal(
            k4c_vector_collect(&iter, sizeof(int), k4c_allocator, &out),
            K4C_STATUS_OK
        )) {
        return 1;
    }

    if (k4c_vector_size(out) != 25) {
        return 1;
    }
    if (k4c_vector_capacity(out) < 25) {
        return 1;
    }

    k4c_vector_destroy(out);
    k4c_vector_destroy(v);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(vector_collect_map_changes_type) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_vector *v = NULL;
    if (k4c_test_equal(k4c_vector_create(sizeof(int), k4c_allocator, &v), K4C_STATUS_OK)) {
        return 1;
    }
    int values[] = {1, 2, 3};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        if (k4c_test_status_ok(k4c_vector_push(v, &values[i]))) {
            return 1;
        }
    }

    k4c_iterator iter = k4c_vector_get_iterator(v);
    k4c_vector *out = NULL;
    if (k4c_test_equal(
            k4c_vector_collect_map(&iter, sizeof(double), int_to_double, NULL, k4c_allocator, &out),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    k4c_vector_destroy(v);

    if (k4c_vector_size(out) != sizeof(values) / sizeof(values[0])) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        const double *value = (const double *)k4c_vector_get(out, i);
        if (k4c_test_equal((int)*value, values[i]) != 0) {
            return 1;
        }
    }

    k4c_vector_destroy(out);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(binary_search_bounds) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_vector *v = NULL;
    if (k4c_test_equal(k4c_vector_create(sizeof(int), k4c_allocator, &v), K4C_STATUS_OK)) {
        return 1;
    }
    int values[] = {1, 2, 2, 2, 4, 8};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        if (k4c_test_status_ok(k4c_vector_push(v, &values[i]))) {
            return 1;
        }
    }

    int key = 2;
    if (k4c_vector_lower_bound(v, &key, cmp_int) != 1) {
        return 1;
    }
    if (k4c_vector_upper_bound(v, &key, cmp_int) != 4) {
        return 1;
    }
    if (k4c_vector_binary_search(v, &key, cmp_int) != 1) {
        return 1;
    }

    key = 3;
    if (k4c_vector_lower_bound(v, &key, cmp_int) != 4) {
        return 1;
    }
    if (k4c_vector_upper_bound(v, &key, cmp_int) != 4) {
        return 1;
    }
    if (k4c_vector_binary_search(v, &key, cmp_int) != k4c_vector_size(v)) {
        return 1;
    }

    key = 0;
    if (k4c_vector_lower_bound(v, &key, cmp_int) != 0) {
        return 1;
    }
    key = 9;
    if (k4c_vector_upper_bound(v, &key, cmp_int) != k4c_vector_size(v)) {
        return 1;
    }

    k4c_vector_destroy(v);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST_MAIN(
    K4C_TEST_CASE(init),
    K4C_TEST_CASE(pop),
    K4C_TEST_CASE(push_single_element),
    K4C_TEST_CASE(push_grows_storage),
    K4C_TEST_CASE(push_preserves_existing_items_after_growth),
    K4C_TEST_CASE(reserve_and_data_access),
    K4C_TEST_CASE(iterator_walks_vector),
    K4C_TEST_CASE(vector_for_each_macro_walks_items),
    K4C_TEST_CASE(custom_callback_iterator_takes_ten_at_a_time),
    K4C_TEST_CASE(vector_collect_copies_items),
    K4C_TEST_CASE(vector_collect_reserves_from_size_hint),
    K4C_TEST_CASE(vector_collect_map_changes_type),
    K4C_TEST_CASE(binary_search_bounds)
)
