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

#include "ckit/testing.h"
#include "ckit/memory/allocators/test_allocator.h"
#include "ckit/datastruct/binary_heap.h"
#include <stddef.h>

static int cmp_int_asc(const void *a, const void *b) {
    int lhs = *(const int *)a;
    int rhs = *(const int *)b;
    return (lhs > rhs) - (lhs < rhs);
}

CK_TEST(init) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_binary_heap *heap;

    heap =
        ck_binary_heap_create(sizeof(int), cmp_int_asc, ck_test_allocator_adapter(&test_allocator));

    CK_TEST_ASSERT_EQ(ck_binary_heap_size(heap), 0);

    ck_binary_heap_destroy(heap);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(peek) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_binary_heap *heap;
    int value = 3;
    const int *out;

    heap =
        ck_binary_heap_create(sizeof(int), cmp_int_asc, ck_test_allocator_adapter(&test_allocator));

    CK_TEST_ASSERT_PTR_NULL(ck_binary_heap_peek(heap));

    ck_binary_heap_push(heap, &value);
    out = (const int *)ck_binary_heap_peek(heap);
    CK_TEST_ASSERT_PTR_NOT_NULL(out);
    CK_TEST_ASSERT_EQ(*out, 3);

    ck_binary_heap_destroy(heap);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(pop) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_binary_heap *heap;
    int values[] = {4, 1, 3};

    heap =
        ck_binary_heap_create(sizeof(int), cmp_int_asc, ck_test_allocator_adapter(&test_allocator));
    for (size_t i = 0; i < 3; i++) {
        ck_binary_heap_push(heap, &values[i]);
    }

    for (size_t i = 0; i < 3; i++) {
        int *out = (int *)ck_binary_heap_pop(heap);
        CK_TEST_ASSERT_PTR_NOT_NULL(out);
    }

    CK_TEST_ASSERT_PTR_NULL(ck_binary_heap_pop(heap));

    ck_binary_heap_destroy(heap);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(push) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_binary_heap *heap;
    int values[] = {5, 2, 8, 1};
    const int *top;

    heap =
        ck_binary_heap_create(sizeof(int), cmp_int_asc, ck_test_allocator_adapter(&test_allocator));
    for (size_t i = 0; i < 4; i++) {
        ck_binary_heap_push(heap, &values[i]);
    }

    top = (const int *)ck_binary_heap_peek(heap);
    CK_TEST_ASSERT_PTR_NOT_NULL(top);
    CK_TEST_ASSERT_EQ(*top, 1);
    CK_TEST_ASSERT_EQ(ck_binary_heap_size(heap), 4);

    ck_binary_heap_destroy(heap);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST_MAIN(CK_TEST_CASE(init), CK_TEST_CASE(peek), CK_TEST_CASE(pop), CK_TEST_CASE(push))
