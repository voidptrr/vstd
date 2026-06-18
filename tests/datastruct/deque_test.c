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

#include "vstd/assert.h"
#include "vstd/datastruct/deque.h"
#include "vstd/memory/allocators/test_allocator.h"
#include "vstd/testing.h"

VS_TEST(init) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_deque *q;

    q = vs_deque_create(sizeof(int), vs_test_allocator_adapter(&test_allocator));
    VS_ASSERT_EQ(vs_deque_size(q), 0);

    vs_deque_destroy(q);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(peekback) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_deque *q;
    int first = 1;
    int second = 2;
    const int *out;

    q = vs_deque_create(sizeof(int), vs_test_allocator_adapter(&test_allocator));
    vs_deque_push(q, &first);
    vs_deque_push(q, &second);

    out = (const int *)vs_deque_peekback(q);
    VS_ASSERT_PTR_NOT_NULL(out);
    VS_ASSERT_EQ(*out, second);

    vs_deque_destroy(q);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(peekleft) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_deque *q;
    int first = 1;
    int second = 2;
    const int *out;

    q = vs_deque_create(sizeof(int), vs_test_allocator_adapter(&test_allocator));
    vs_deque_push(q, &first);
    vs_deque_push(q, &second);

    out = (const int *)vs_deque_peekleft(q);
    VS_ASSERT_PTR_NOT_NULL(out);
    VS_ASSERT_EQ(*out, first);

    vs_deque_destroy(q);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(popback) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_deque *q;
    int first = 1;
    int second = 2;
    int *out;

    q = vs_deque_create(sizeof(int), vs_test_allocator_adapter(&test_allocator));
    vs_deque_push(q, &first);
    vs_deque_push(q, &second);

    out = (int *)vs_deque_popback(q);
    VS_ASSERT_PTR_NOT_NULL(out);
    VS_ASSERT_EQ(*out, second);

    vs_deque_destroy(q);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(popleft) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_deque *q;
    int values[] = {10, 20};
    int *out;

    q = vs_deque_create(sizeof(int), vs_test_allocator_adapter(&test_allocator));
    VS_ASSERT_PTR_NULL(vs_deque_popleft(q));

    vs_deque_push(q, &values[0]);
    vs_deque_push(q, &values[1]);
    out = (int *)vs_deque_popleft(q);
    VS_ASSERT_PTR_NOT_NULL(out);
    VS_ASSERT_EQ(*out, values[0]);

    vs_deque_destroy(q);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(push) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_deque *q;
    int value = 42;

    q = vs_deque_create(sizeof(int), vs_test_allocator_adapter(&test_allocator));
    vs_deque_push(q, &value);

    const int *out = vs_deque_peekback(q);
    VS_ASSERT_EQ(vs_deque_size(q), 1);
    VS_ASSERT_PTR_NOT_NULL(out);
    VS_ASSERT_EQ(*out, value);

    vs_deque_destroy(q);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(pushfront) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_deque *q;
    int first = 1;
    int second = 2;
    int *out;

    q = vs_deque_create(sizeof(int), vs_test_allocator_adapter(&test_allocator));
    vs_deque_push(q, &first);
    vs_deque_pushfront(q, &second);

    out = (int *)vs_deque_popleft(q);
    VS_ASSERT_PTR_NOT_NULL(out);
    VS_ASSERT_EQ(*out, second);

    vs_deque_destroy(q);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST_MAIN(
    VS_TEST_CASE(init),
    VS_TEST_CASE(peekback),
    VS_TEST_CASE(peekleft),
    VS_TEST_CASE(popback),
    VS_TEST_CASE(popleft),
    VS_TEST_CASE(push),
    VS_TEST_CASE(pushfront)
)
