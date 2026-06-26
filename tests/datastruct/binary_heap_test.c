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

#include "vstd/datastruct/binary_heap.h"
#include "vstd/datastruct/iterator.h"
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

VS_TEST(init) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_binary_heap *heap = vs_binary_heap_create(sizeof(int), cmp_int_asc, allocator);

    if (vs_binary_heap_size(heap) != 0) {
        return 1;
    }

    vs_binary_heap_destroy(heap);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(peek) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_binary_heap *heap = vs_binary_heap_create(sizeof(int), cmp_int_asc, allocator);
    int value = 3;

    if (vs_test_null(vs_binary_heap_peek(heap)) != 0) {
        return 1;
    }

    vs_binary_heap_push(heap, &value);
    const int *out = (const int *)vs_binary_heap_peek(heap);
    if (vs_test_not_null(out) != 0) {
        return 1;
    }
    if (vs_test_equal(*out, 3) != 0) {
        return 1;
    }

    vs_binary_heap_destroy(heap);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(pop) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_binary_heap *heap = vs_binary_heap_create(sizeof(int), cmp_int_asc, allocator);
    int values[] = {4, 1, 3};

    for (size_t i = 0; i < 3; i++) {
        vs_binary_heap_push(heap, &values[i]);
    }

    int expected[] = {1, 3, 4};
    for (size_t i = 0; i < 3; i++) {
        int *out = (int *)vs_binary_heap_pop(heap);
        if (vs_test_not_null(out) != 0) {
            return 1;
        }
        if (vs_test_equal(*out, expected[i]) != 0) {
            return 1;
        }
    }

    if (vs_test_null(vs_binary_heap_pop(heap)) != 0) {
        return 1;
    }

    vs_binary_heap_destroy(heap);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(pop_uses_custom_comparator_order) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_binary_heap *heap = vs_binary_heap_create(sizeof(int), cmp_int_desc, allocator);
    int values[] = {5, 2, 8, 1};
    int expected[] = {8, 5, 2, 1};

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        vs_binary_heap_push(heap, &values[i]);
    }

    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        int *out = (int *)vs_binary_heap_pop(heap);
        if (vs_test_not_null(out) != 0) {
            return 1;
        }
        if (vs_test_equal(*out, expected[i]) != 0) {
            return 1;
        }
    }

    vs_binary_heap_destroy(heap);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(push) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_binary_heap *heap = vs_binary_heap_create(sizeof(int), cmp_int_asc, allocator);
    int values[] = {5, 2, 8, 1};

    for (size_t i = 0; i < 4; i++) {
        vs_binary_heap_push(heap, &values[i]);
    }

    const int *top = (const int *)vs_binary_heap_peek(heap);
    if (vs_test_not_null(top) != 0) {
        return 1;
    }
    if (vs_test_equal(*top, 1) != 0) {
        return 1;
    }
    if (vs_binary_heap_size(heap) != 4) {
        return 1;
    }

    vs_binary_heap_destroy(heap);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(default_byte_ordering) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_binary_heap *heap = vs_binary_heap_create(sizeof(uint8_t), NULL, allocator);
    uint8_t values[] = {5, 2, 8, 1};

    for (size_t i = 0; i < 4; i++) {
        vs_binary_heap_push(heap, &values[i]);
    }

    const uint8_t *top = (const uint8_t *)vs_binary_heap_peek(heap);
    if (vs_test_not_null(top) != 0) {
        return 1;
    }
    if (vs_test_equal(*top, 1) != 0) {
        return 1;
    }

    vs_binary_heap_destroy(heap);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(custom_comparator) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_binary_heap *heap = vs_binary_heap_create(sizeof(int), cmp_int_desc, allocator);
    int values[] = {5, 2, 8, 1};

    for (size_t i = 0; i < 4; i++) {
        vs_binary_heap_push(heap, &values[i]);
    }

    const int *top = (const int *)vs_binary_heap_peek(heap);
    if (vs_test_not_null(top) != 0) {
        return 1;
    }
    if (vs_test_equal(*top, 8) != 0) {
        return 1;
    }

    vs_binary_heap_destroy(heap);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(iterator_walks_backing_storage) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_binary_heap *heap = vs_binary_heap_create(sizeof(int), cmp_int_asc, allocator);
    int values[] = {5, 2, 8, 1};
    const int *out;
    int sum = 0;
    size_t count = 0;

    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
        vs_binary_heap_push(heap, &values[i]);
    }

    vs_iterator iter = vs_binary_heap_get_iterator(heap);
    while ((out = (const int *)vs_iterator_next(&iter)) != NULL) {
        sum += *out;
        count += 1;
    }
    if (count != sizeof(values) / sizeof(values[0])) {
        return 1;
    }
    if (vs_test_equal(sum, 16) != 0) {
        return 1;
    }

    vs_binary_heap_destroy(heap);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST_MAIN(
    VS_TEST_CASE(init),
    VS_TEST_CASE(peek),
    VS_TEST_CASE(pop),
    VS_TEST_CASE(pop_uses_custom_comparator_order),
    VS_TEST_CASE(push),
    VS_TEST_CASE(default_byte_ordering),
    VS_TEST_CASE(custom_comparator),
    VS_TEST_CASE(iterator_walks_backing_storage)
)
