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

#include "vstd/ds/deque.h"
#include "vstd/ds/iterator.h"
#include "vstd/ds/vector.h"
#include "vstd/error.h"
#include "vstd/memory/test_allocator.h"
#include "vstd/testing.h"

TEST(init) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    deque *q = NULL;
    if (test_equal(deque_create(sizeof(int), allocator, &q), STATUS_OK)) {
        return 1;
    }

    if (deque_size(q) != 0) {
        return 1;
    }

    deque_destroy(q);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(peekback) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    deque *q = NULL;
    if (test_equal(deque_create(sizeof(int), allocator, &q), STATUS_OK)) {
        return 1;
    }
    int first = 1;
    int second = 2;

    if (test_status_ok(deque_push(q, &first))) {

        return 1;
    }
    if (test_status_ok(deque_push(q, &second))) {
        return 1;
    }

    const int *out = (const int *)deque_peekback(q);
    if (test_not_null(out) != 0) {
        return 1;
    }
    if (test_equal(*out, second) != 0) {
        return 1;
    }

    deque_destroy(q);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(peekleft) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    deque *q = NULL;
    if (test_equal(deque_create(sizeof(int), allocator, &q), STATUS_OK)) {
        return 1;
    }
    int first = 1;
    int second = 2;

    if (test_status_ok(deque_push(q, &first))) {

        return 1;
    }
    if (test_status_ok(deque_push(q, &second))) {
        return 1;
    }

    const int *out = (const int *)deque_peekleft(q);
    if (test_not_null(out) != 0) {
        return 1;
    }
    if (test_equal(*out, first) != 0) {
        return 1;
    }

    deque_destroy(q);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(popback) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    deque *q = NULL;
    if (test_equal(deque_create(sizeof(int), allocator, &q), STATUS_OK)) {
        return 1;
    }
    int first = 1;
    int second = 2;

    if (test_status_ok(deque_push(q, &first))) {

        return 1;
    }
    if (test_status_ok(deque_push(q, &second))) {
        return 1;
    }

    int *out = (int *)deque_popback(q);
    if (test_not_null(out) != 0) {
        return 1;
    }
    if (test_equal(*out, second) != 0) {
        return 1;
    }

    deque_destroy(q);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(popleft) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    deque *q = NULL;
    if (test_equal(deque_create(sizeof(int), allocator, &q), STATUS_OK)) {
        return 1;
    }
    int values[] = {10, 20};

    if (test_null(deque_popleft(q)) != 0) {
        return 1;
    }

    if (test_status_ok(deque_push(q, &values[0]))) {

        return 1;
    }
    if (test_status_ok(deque_push(q, &values[1]))) {
        return 1;
    }
    int *out = (int *)deque_popleft(q);
    if (test_not_null(out) != 0) {
        return 1;
    }
    if (test_equal(*out, values[0]) != 0) {
        return 1;
    }

    deque_destroy(q);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(push) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    deque *q = NULL;
    if (test_equal(deque_create(sizeof(int), allocator, &q), STATUS_OK)) {
        return 1;
    }
    int value = 42;

    if (test_status_ok(deque_push(q, &value))) {

        return 1;
    }

    const int *out = deque_peekback(q);
    if (deque_size(q) != 1) {
        return 1;
    }
    if (test_not_null(out) != 0) {
        return 1;
    }
    if (test_equal(*out, value) != 0) {
        return 1;
    }

    deque_destroy(q);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(pushfront) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    deque *q = NULL;
    if (test_equal(deque_create(sizeof(int), allocator, &q), STATUS_OK)) {
        return 1;
    }
    int first = 1;
    int second = 2;

    if (test_status_ok(deque_push(q, &first))) {

        return 1;
    }
    if (test_status_ok(deque_pushfront(q, &second))) {
        return 1;
    }

    int *out = (int *)deque_popleft(q);
    if (test_not_null(out) != 0) {
        return 1;
    }
    if (test_equal(*out, second) != 0) {
        return 1;
    }

    deque_destroy(q);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(iterator_walks_front_to_back) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    deque *q = NULL;
    if (test_equal(deque_create(sizeof(int), allocator, &q), STATUS_OK)) {
        return 1;
    }
    const int *out;
    int expected[] = {1, 2, 3, 4};
    size_t index = 0;

    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        if (test_status_ok(deque_push(q, &expected[i]))) {
            return 1;
        }
    }

    iterator iter = deque_get_iterator(q);
    while ((out = (const int *)iterator_next(&iter)) != NULL) {
        if (index >= sizeof(expected) / sizeof(expected[0])) {
            return 1;
        }
        if (test_equal(*out, expected[index]) != 0) {
            return 1;
        }
        index += 1;
    }
    if (index != sizeof(expected) / sizeof(expected[0])) {
        return 1;
    }

    deque_destroy(q);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(iterator_collect_copies_items) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    deque *q = NULL;
    if (test_equal(deque_create(sizeof(int), allocator, &q), STATUS_OK)) {
        return 1;
    }
    int expected[] = {1, 2, 3, 4};

    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        if (test_status_ok(deque_push(q, &expected[i]))) {
            return 1;
        }
    }

    iterator iter = deque_get_iterator(q);
    vector *out = NULL;
    if (test_equal(iterator_collect(&iter, sizeof(int), allocator, &out), STATUS_OK)) {
        return 1;
    }
    deque_destroy(q);

    if (vector_size(out) != sizeof(expected) / sizeof(expected[0])) {
        return 1;
    }
    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        if (test_equal(*(const int *)vector_get_const(out, i), expected[i]) != 0) {
            return 1;
        }
    }

    vector_destroy(out);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(iterator_walks_items_after_growth) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    deque *q = NULL;
    if (test_equal(deque_create(sizeof(int), allocator, &q), STATUS_OK)) {
        return 1;
    }

    for (int i = 0; i < 20; i++) {
        if (test_status_ok(deque_push(q, &i))) {
            return 1;
        }
    }

    size_t index = 0;
    iterator iter = deque_get_iterator(q);
    const int *out = NULL;
    while ((out = (const int *)iterator_next(&iter)) != NULL) {
        if (test_equal(*out, (int)index) != 0) {
            return 1;
        }
        index += 1;
    }

    if (test_equal(index, (size_t)20) != 0) {
        return 1;
    }

    deque_destroy(q);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST_MAIN(
    TEST_CASE(init),
    TEST_CASE(peekback),
    TEST_CASE(peekleft),
    TEST_CASE(popback),
    TEST_CASE(popleft),
    TEST_CASE(push),
    TEST_CASE(pushfront),
    TEST_CASE(iterator_walks_front_to_back),
    TEST_CASE(iterator_collect_copies_items),
    TEST_CASE(iterator_walks_items_after_growth)
)
