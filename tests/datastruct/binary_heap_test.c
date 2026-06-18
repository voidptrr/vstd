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

#include "vstd/assert.h"
#include "vstd/datastruct/binary_heap.h"
#include "vstd/memory/allocators/test_allocator.h"
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
    vs_test_allocator_init(&test_allocator);
    vs_binary_heap *heap;

    heap =
        vs_binary_heap_create(sizeof(int), cmp_int_asc, vs_test_allocator_adapter(&test_allocator));

    VS_ASSERT_EQ(vs_binary_heap_size(heap), 0);

    vs_binary_heap_destroy(heap);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(peek) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_binary_heap *heap;
    int value = 3;
    const int *out;

    heap =
        vs_binary_heap_create(sizeof(int), cmp_int_asc, vs_test_allocator_adapter(&test_allocator));

    VS_ASSERT_PTR_NULL(vs_binary_heap_peek(heap));

    vs_binary_heap_push(heap, &value);
    out = (const int *)vs_binary_heap_peek(heap);
    VS_ASSERT_PTR_NOT_NULL(out);
    VS_ASSERT_EQ(*out, 3);

    vs_binary_heap_destroy(heap);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(pop) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_binary_heap *heap;
    int values[] = {4, 1, 3};

    heap =
        vs_binary_heap_create(sizeof(int), cmp_int_asc, vs_test_allocator_adapter(&test_allocator));
    for (size_t i = 0; i < 3; i++) {
        vs_binary_heap_push(heap, &values[i]);
    }

    for (size_t i = 0; i < 3; i++) {
        int *out = (int *)vs_binary_heap_pop(heap);
        VS_ASSERT_PTR_NOT_NULL(out);
    }

    VS_ASSERT_PTR_NULL(vs_binary_heap_pop(heap));

    vs_binary_heap_destroy(heap);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(push) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_binary_heap *heap;
    int values[] = {5, 2, 8, 1};
    const int *top;

    heap =
        vs_binary_heap_create(sizeof(int), cmp_int_asc, vs_test_allocator_adapter(&test_allocator));
    for (size_t i = 0; i < 4; i++) {
        vs_binary_heap_push(heap, &values[i]);
    }

    top = (const int *)vs_binary_heap_peek(heap);
    VS_ASSERT_PTR_NOT_NULL(top);
    VS_ASSERT_EQ(*top, 1);
    VS_ASSERT_EQ(vs_binary_heap_size(heap), 4);

    vs_binary_heap_destroy(heap);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(default_byte_ordering) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_binary_heap *heap;
    uint8_t values[] = {5, 2, 8, 1};
    const uint8_t *top;

    heap = vs_binary_heap_create(sizeof(uint8_t), NULL, vs_test_allocator_adapter(&test_allocator));
    for (size_t i = 0; i < 4; i++) {
        vs_binary_heap_push(heap, &values[i]);
    }

    top = (const uint8_t *)vs_binary_heap_peek(heap);
    VS_ASSERT_PTR_NOT_NULL(top);
    VS_ASSERT_EQ(*top, 1);

    vs_binary_heap_destroy(heap);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(custom_comparator) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_binary_heap *heap;
    int values[] = {5, 2, 8, 1};
    const int *top;

    heap = vs_binary_heap_create(
        sizeof(int),
        cmp_int_desc,
        vs_test_allocator_adapter(&test_allocator)
    );
    for (size_t i = 0; i < 4; i++) {
        vs_binary_heap_push(heap, &values[i]);
    }

    top = (const int *)vs_binary_heap_peek(heap);
    VS_ASSERT_PTR_NOT_NULL(top);
    VS_ASSERT_EQ(*top, 8);

    vs_binary_heap_destroy(heap);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST_MAIN(
    VS_TEST_CASE(init),
    VS_TEST_CASE(peek),
    VS_TEST_CASE(pop),
    VS_TEST_CASE(push),
    VS_TEST_CASE(default_byte_ordering),
    VS_TEST_CASE(custom_comparator)
)
