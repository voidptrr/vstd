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

#include "vstd/ds/binary_heap.h"
#include "vstd/ds/iterator.h"
#include "vstd/error.h"
#include "vstd/memory/test_allocator.h"
#include "vstd/testing.h"

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

TEST(init) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    binary_heap *heap = NULL;
    if (test_equal(binary_heap_create(sizeof(int), cmp_int_asc, allocator, &heap), STATUS_OK)) {
        return 1;
    }

    if (binary_heap_size(heap) != 0) {
        return 1;
    }

    binary_heap_destroy(heap);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(peek) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    binary_heap *heap = NULL;
    if (test_equal(binary_heap_create(sizeof(int), cmp_int_asc, allocator, &heap), STATUS_OK)) {
        return 1;
    }
    int value = 3;

    if (test_null(binary_heap_peek(heap)) != 0) {
        return 1;
    }

    if (test_status_ok(binary_heap_push(heap, &value))) {

        return 1;
    }
    const int *out = (const int *)binary_heap_peek(heap);
    if (test_not_null(out) != 0) {
        return 1;
    }
    if (test_equal(*out, 3) != 0) {
        return 1;
    }

    binary_heap_destroy(heap);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(pop) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    binary_heap *heap = NULL;
    if (test_equal(binary_heap_create(sizeof(int), cmp_int_asc, allocator, &heap), STATUS_OK)) {
        return 1;
    }
    int values[] = {4, 1, 3};

    for (size_t i = 0; i < 3; i++) {
        if (test_status_ok(binary_heap_push(heap, &values[i]))) {
            return 1;
        }
    }

    int expected[] = {1, 3, 4};
    for (size_t i = 0; i < 3; i++) {
        int *out = (int *)binary_heap_pop(heap);
        if (test_not_null(out) != 0) {
            return 1;
        }
        if (test_equal(*out, expected[i]) != 0) {
            return 1;
        }
    }

    if (test_null(binary_heap_pop(heap)) != 0) {
        return 1;
    }

    binary_heap_destroy(heap);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(pop_uses_custom_comparator_order) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    binary_heap *heap = NULL;
    if (test_equal(binary_heap_create(sizeof(int), cmp_int_desc, allocator, &heap), STATUS_OK)) {
        return 1;
    }
    int values[] = {5, 2, 8, 1};
    int expected[] = {8, 5, 2, 1};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        if (test_status_ok(binary_heap_push(heap, &values[i]))) {
            return 1;
        }
    }

    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        int *out = (int *)binary_heap_pop(heap);
        if (test_not_null(out) != 0) {
            return 1;
        }
        if (test_equal(*out, expected[i]) != 0) {
            return 1;
        }
    }

    binary_heap_destroy(heap);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(push) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    binary_heap *heap = NULL;
    if (test_equal(binary_heap_create(sizeof(int), cmp_int_asc, allocator, &heap), STATUS_OK)) {
        return 1;
    }
    int values[] = {5, 2, 8, 1};

    for (size_t i = 0; i < 4; i++) {
        if (test_status_ok(binary_heap_push(heap, &values[i]))) {
            return 1;
        }
    }

    const int *top = (const int *)binary_heap_peek(heap);
    if (test_not_null(top) != 0) {
        return 1;
    }
    if (test_equal(*top, 1) != 0) {
        return 1;
    }
    if (binary_heap_size(heap) != 4) {
        return 1;
    }

    binary_heap_destroy(heap);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(default_byte_ordering) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    binary_heap *heap = NULL;
    if (test_equal(binary_heap_create(sizeof(uint8_t), NULL, allocator, &heap), STATUS_OK)) {
        return 1;
    }
    uint8_t values[] = {5, 2, 8, 1};

    for (size_t i = 0; i < 4; i++) {
        if (test_status_ok(binary_heap_push(heap, &values[i]))) {
            return 1;
        }
    }

    const uint8_t *top = (const uint8_t *)binary_heap_peek(heap);
    if (test_not_null(top) != 0) {
        return 1;
    }
    if (test_equal(*top, 1) != 0) {
        return 1;
    }

    binary_heap_destroy(heap);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(custom_comparator) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    binary_heap *heap = NULL;
    if (test_equal(binary_heap_create(sizeof(int), cmp_int_desc, allocator, &heap), STATUS_OK)) {
        return 1;
    }
    int values[] = {5, 2, 8, 1};

    for (size_t i = 0; i < 4; i++) {
        if (test_status_ok(binary_heap_push(heap, &values[i]))) {
            return 1;
        }
    }

    const int *top = (const int *)binary_heap_peek(heap);
    if (test_not_null(top) != 0) {
        return 1;
    }
    if (test_equal(*top, 8) != 0) {
        return 1;
    }

    binary_heap_destroy(heap);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(iterator_walks_backing_storage) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    binary_heap *heap = NULL;
    if (test_equal(binary_heap_create(sizeof(int), cmp_int_asc, allocator, &heap), STATUS_OK)) {
        return 1;
    }
    int values[] = {5, 2, 8, 1};
    const int *out;
    int sum = 0;
    size_t count = 0;

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        if (test_status_ok(binary_heap_push(heap, &values[i]))) {
            return 1;
        }
    }

    iterator iter = binary_heap_get_iterator(heap);
    while ((out = (const int *)iterator_next(&iter)) != NULL) {
        sum += *out;
        count += 1;
    }
    if (count != sizeof(values) / sizeof(values[0])) {
        return 1;
    }
    if (test_equal(sum, 16) != 0) {
        return 1;
    }

    binary_heap_destroy(heap);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST_MAIN(
    TEST_CASE(init),
    TEST_CASE(peek),
    TEST_CASE(pop),
    TEST_CASE(pop_uses_custom_comparator_order),
    TEST_CASE(push),
    TEST_CASE(default_byte_ordering),
    TEST_CASE(custom_comparator),
    TEST_CASE(iterator_walks_backing_storage)
)
