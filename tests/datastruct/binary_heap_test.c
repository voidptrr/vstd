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
#include <stdint.h>

#include "k4c/allocators/allocator.h"
#include "k4c/allocators/test_allocator.h"
#include "k4c/datastruct/binary_heap.h"
#include "k4c/datastruct/iterator.h"
#include "k4c/error.h"
#include "k4c/testing.h"

static int cmp_int_asc(const void *a, const void *b) {
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    return (lhs > rhs) - (lhs < rhs);
}

static int cmp_int_desc(const void *a, const void *b) {
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    return (rhs > lhs) - (rhs < lhs);
}

K4C_TEST(init) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_binary_heap *k4c_heap = NULL;
    if (k4c_test_equal(
            k4c_binary_heap_create(sizeof(int), cmp_int_asc, k4c_allocator, &k4c_heap),
            K4C_STATUS_OK
        )) {
        return 1;
    }

    if (k4c_binary_heap_size(k4c_heap) != 0) {
        return 1;
    }

    k4c_binary_heap_destroy(k4c_heap);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(peek) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_binary_heap *k4c_heap = NULL;
    if (k4c_test_equal(
            k4c_binary_heap_create(sizeof(int), cmp_int_asc, k4c_allocator, &k4c_heap),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    int value = 3;

    if (k4c_test_null(k4c_binary_heap_peek(k4c_heap)) != 0) {
        return 1;
    }

    if (k4c_test_status_ok(k4c_binary_heap_push(k4c_heap, &value))) {

        return 1;
    }
    const int *out = (const int *)k4c_binary_heap_peek(k4c_heap);
    if (k4c_test_not_null(out) != 0) {
        return 1;
    }
    if (k4c_test_equal(*out, 3) != 0) {
        return 1;
    }

    k4c_binary_heap_destroy(k4c_heap);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(pop) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_binary_heap *k4c_heap = NULL;
    if (k4c_test_equal(
            k4c_binary_heap_create(sizeof(int), cmp_int_asc, k4c_allocator, &k4c_heap),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    int values[] = {4, 1, 3};

    for (size_t i = 0; i < 3; i++) {
        if (k4c_test_status_ok(k4c_binary_heap_push(k4c_heap, &values[i]))) {
            return 1;
        }
    }

    int expected[] = {1, 3, 4};
    for (size_t i = 0; i < 3; i++) {
        int *out = (int *)k4c_binary_heap_pop(k4c_heap);
        if (k4c_test_not_null(out) != 0) {
            return 1;
        }
        if (k4c_test_equal(*out, expected[i]) != 0) {
            return 1;
        }
    }

    if (k4c_test_null(k4c_binary_heap_pop(k4c_heap)) != 0) {
        return 1;
    }

    k4c_binary_heap_destroy(k4c_heap);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(pop_uses_custom_comparator_order) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_binary_heap *k4c_heap = NULL;
    if (k4c_test_equal(
            k4c_binary_heap_create(sizeof(int), cmp_int_desc, k4c_allocator, &k4c_heap),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    int values[] = {5, 2, 8, 1};
    int expected[] = {8, 5, 2, 1};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        if (k4c_test_status_ok(k4c_binary_heap_push(k4c_heap, &values[i]))) {
            return 1;
        }
    }

    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        int *out = (int *)k4c_binary_heap_pop(k4c_heap);
        if (k4c_test_not_null(out) != 0) {
            return 1;
        }
        if (k4c_test_equal(*out, expected[i]) != 0) {
            return 1;
        }
    }

    k4c_binary_heap_destroy(k4c_heap);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(push) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_binary_heap *k4c_heap = NULL;
    if (k4c_test_equal(
            k4c_binary_heap_create(sizeof(int), cmp_int_asc, k4c_allocator, &k4c_heap),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    int values[] = {5, 2, 8, 1};

    for (size_t i = 0; i < 4; i++) {
        if (k4c_test_status_ok(k4c_binary_heap_push(k4c_heap, &values[i]))) {
            return 1;
        }
    }

    const int *top = (const int *)k4c_binary_heap_peek(k4c_heap);
    if (k4c_test_not_null(top) != 0) {
        return 1;
    }
    if (k4c_test_equal(*top, 1) != 0) {
        return 1;
    }
    if (k4c_binary_heap_size(k4c_heap) != 4) {
        return 1;
    }

    k4c_binary_heap_destroy(k4c_heap);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(default_byte_ordering) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_binary_heap *k4c_heap = NULL;
    if (k4c_test_equal(
            k4c_binary_heap_create(sizeof(uint8_t), NULL, k4c_allocator, &k4c_heap),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    uint8_t values[] = {5, 2, 8, 1};

    for (size_t i = 0; i < 4; i++) {
        if (k4c_test_status_ok(k4c_binary_heap_push(k4c_heap, &values[i]))) {
            return 1;
        }
    }

    const uint8_t *top = (const uint8_t *)k4c_binary_heap_peek(k4c_heap);
    if (k4c_test_not_null(top) != 0) {
        return 1;
    }
    if (k4c_test_equal(*top, 1) != 0) {
        return 1;
    }

    k4c_binary_heap_destroy(k4c_heap);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(custom_comparator) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_binary_heap *k4c_heap = NULL;
    if (k4c_test_equal(
            k4c_binary_heap_create(sizeof(int), cmp_int_desc, k4c_allocator, &k4c_heap),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    int values[] = {5, 2, 8, 1};

    for (size_t i = 0; i < 4; i++) {
        if (k4c_test_status_ok(k4c_binary_heap_push(k4c_heap, &values[i]))) {
            return 1;
        }
    }

    const int *top = (const int *)k4c_binary_heap_peek(k4c_heap);
    if (k4c_test_not_null(top) != 0) {
        return 1;
    }
    if (k4c_test_equal(*top, 8) != 0) {
        return 1;
    }

    k4c_binary_heap_destroy(k4c_heap);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(iterator_walks_backing_storage) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    k4c_binary_heap *k4c_heap = NULL;
    if (k4c_test_equal(
            k4c_binary_heap_create(sizeof(int), cmp_int_asc, k4c_allocator, &k4c_heap),
            K4C_STATUS_OK
        )) {
        return 1;
    }
    int values[] = {5, 2, 8, 1};
    const int *out;
    int sum = 0;
    size_t count = 0;

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        if (k4c_test_status_ok(k4c_binary_heap_push(k4c_heap, &values[i]))) {
            return 1;
        }
    }

    k4c_iterator iter = k4c_binary_heap_get_iterator(k4c_heap);
    while ((out = (const int *)k4c_iterator_next(&iter)) != NULL) {
        sum += *out;
        count += 1;
    }
    if (count != sizeof(values) / sizeof(values[0])) {
        return 1;
    }
    if (k4c_test_equal(sum, 16) != 0) {
        return 1;
    }

    k4c_binary_heap_destroy(k4c_heap);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST_MAIN(
    K4C_TEST_CASE(init),
    K4C_TEST_CASE(peek),
    K4C_TEST_CASE(pop),
    K4C_TEST_CASE(pop_uses_custom_comparator_order),
    K4C_TEST_CASE(push),
    K4C_TEST_CASE(default_byte_ordering),
    K4C_TEST_CASE(custom_comparator),
    K4C_TEST_CASE(iterator_walks_backing_storage)
)
