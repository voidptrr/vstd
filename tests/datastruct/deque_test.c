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
#include "k4c/datastruct/deque.h"
#include "k4c/datastruct/iterator.h"
#include "k4c/datastruct/vector.h"
#include "k4c/error.h"
#include "k4c/testing.h"

K4C_TEST(init) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_deque *q = NULL;
    if (k4c_test_equal(k4c_deque_create(sizeof(int), k4c_allocator, &q), K4C_STATUS_OK)) {
        return 1;
    }

    if (k4c_deque_size(q) != 0) {
        return 1;
    }

    k4c_deque_destroy(q);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(peekback) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_deque *q = NULL;
    if (k4c_test_equal(k4c_deque_create(sizeof(int), k4c_allocator, &q), K4C_STATUS_OK)) {
        return 1;
    }
    int first = 1;
    int second = 2;

    if (k4c_test_status_ok(k4c_deque_push(q, &first))) {

        return 1;
    }
    if (k4c_test_status_ok(k4c_deque_push(q, &second))) {
        return 1;
    }

    const int *out = (const int *)k4c_deque_peekback(q);
    if (k4c_test_not_null(out) != 0) {
        return 1;
    }
    if (k4c_test_equal(*out, second) != 0) {
        return 1;
    }

    k4c_deque_destroy(q);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(peekleft) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_deque *q = NULL;
    if (k4c_test_equal(k4c_deque_create(sizeof(int), k4c_allocator, &q), K4C_STATUS_OK)) {
        return 1;
    }
    int first = 1;
    int second = 2;

    if (k4c_test_status_ok(k4c_deque_push(q, &first))) {

        return 1;
    }
    if (k4c_test_status_ok(k4c_deque_push(q, &second))) {
        return 1;
    }

    const int *out = (const int *)k4c_deque_peekleft(q);
    if (k4c_test_not_null(out) != 0) {
        return 1;
    }
    if (k4c_test_equal(*out, first) != 0) {
        return 1;
    }

    k4c_deque_destroy(q);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(popback) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_deque *q = NULL;
    if (k4c_test_equal(k4c_deque_create(sizeof(int), k4c_allocator, &q), K4C_STATUS_OK)) {
        return 1;
    }
    int first = 1;
    int second = 2;

    if (k4c_test_status_ok(k4c_deque_push(q, &first))) {

        return 1;
    }
    if (k4c_test_status_ok(k4c_deque_push(q, &second))) {
        return 1;
    }

    int *out = (int *)k4c_deque_popback(q);
    if (k4c_test_not_null(out) != 0) {
        return 1;
    }
    if (k4c_test_equal(*out, second) != 0) {
        return 1;
    }

    k4c_deque_destroy(q);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(popleft) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_deque *q = NULL;
    if (k4c_test_equal(k4c_deque_create(sizeof(int), k4c_allocator, &q), K4C_STATUS_OK)) {
        return 1;
    }
    int values[] = {10, 20};

    if (k4c_test_null(k4c_deque_popleft(q)) != 0) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_deque_push(q, &values[0]))) {

        return 1;
    }
    if (k4c_test_status_ok(k4c_deque_push(q, &values[1]))) {
        return 1;
    }
    int *out = (int *)k4c_deque_popleft(q);
    if (k4c_test_not_null(out) != 0) {
        return 1;
    }
    if (k4c_test_equal(*out, values[0]) != 0) {
        return 1;
    }

    k4c_deque_destroy(q);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(push) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_deque *q = NULL;
    if (k4c_test_equal(k4c_deque_create(sizeof(int), k4c_allocator, &q), K4C_STATUS_OK)) {
        return 1;
    }
    int value = 42;

    if (k4c_test_status_ok(k4c_deque_push(q, &value))) {

        return 1;
    }

    const int *out = k4c_deque_peekback(q);
    if (k4c_deque_size(q) != 1) {
        return 1;
    }
    if (k4c_test_not_null(out) != 0) {
        return 1;
    }
    if (k4c_test_equal(*out, value) != 0) {
        return 1;
    }

    k4c_deque_destroy(q);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(pushfront) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_deque *q = NULL;
    if (k4c_test_equal(k4c_deque_create(sizeof(int), k4c_allocator, &q), K4C_STATUS_OK)) {
        return 1;
    }
    int first = 1;
    int second = 2;

    if (k4c_test_status_ok(k4c_deque_push(q, &first))) {

        return 1;
    }
    if (k4c_test_status_ok(k4c_deque_pushfront(q, &second))) {
        return 1;
    }

    int *out = (int *)k4c_deque_popleft(q);
    if (k4c_test_not_null(out) != 0) {
        return 1;
    }
    if (k4c_test_equal(*out, second) != 0) {
        return 1;
    }

    k4c_deque_destroy(q);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(iterator_walks_front_to_back) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_deque *q = NULL;
    if (k4c_test_equal(k4c_deque_create(sizeof(int), k4c_allocator, &q), K4C_STATUS_OK)) {
        return 1;
    }
    const int *out;
    int expected[] = {1, 2, 3, 4};
    size_t index = 0;

    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        if (k4c_test_status_ok(k4c_deque_push(q, &expected[i]))) {
            return 1;
        }
    }

    k4c_iterator iter = k4c_deque_get_iterator(q);
    while ((out = (const int *)k4c_iterator_next(&iter)) != NULL) {
        if (index >= sizeof(expected) / sizeof(expected[0])) {
            return 1;
        }
        if (k4c_test_equal(*out, expected[index]) != 0) {
            return 1;
        }
        index += 1;
    }
    if (index != sizeof(expected) / sizeof(expected[0])) {
        return 1;
    }

    k4c_deque_destroy(q);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(vector_collect_copies_items) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_deque *q = NULL;
    if (k4c_test_equal(k4c_deque_create(sizeof(int), k4c_allocator, &q), K4C_STATUS_OK)) {
        return 1;
    }
    int expected[] = {1, 2, 3, 4};

    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        if (k4c_test_status_ok(k4c_deque_push(q, &expected[i]))) {
            return 1;
        }
    }

    k4c_iterator iter = k4c_deque_get_iterator(q);
    k4c_vector *out = NULL;
    if (k4c_test_equal(
            k4c_vector_collect(&iter, sizeof(int), k4c_allocator, &out),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    k4c_deque_destroy(q);

    if (k4c_vector_size(out) != sizeof(expected) / sizeof(expected[0])) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        if (k4c_test_equal(*(const int *)k4c_vector_get(out, i), expected[i]) != 0) {
            return 1;
        }
    }

    k4c_vector_destroy(out);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(iterator_walks_items_after_growth) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_deque *q = NULL;
    if (k4c_test_equal(k4c_deque_create(sizeof(int), k4c_allocator, &q), K4C_STATUS_OK)) {
        return 1;
    }

    for (int i = 0; i < 20; i++) {
        if (k4c_test_status_ok(k4c_deque_push(q, &i))) {
            return 1;
        }
    }

    size_t index = 0;
    k4c_iterator iter = k4c_deque_get_iterator(q);
    const int *out = NULL;
    while ((out = (const int *)k4c_iterator_next(&iter)) != NULL) {
        if (k4c_test_equal(*out, (int)index) != 0) {
            return 1;
        }
        index += 1;
    }

    if (k4c_test_equal(index, (size_t)20) != 0) {
        return 1;
    }

    k4c_deque_destroy(q);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST_MAIN(
    K4C_TEST_CASE(init),
    K4C_TEST_CASE(peekback),
    K4C_TEST_CASE(peekleft),
    K4C_TEST_CASE(popback),
    K4C_TEST_CASE(popleft),
    K4C_TEST_CASE(push),
    K4C_TEST_CASE(pushfront),
    K4C_TEST_CASE(iterator_walks_front_to_back),
    K4C_TEST_CASE(vector_collect_copies_items),
    K4C_TEST_CASE(iterator_walks_items_after_growth)
)
