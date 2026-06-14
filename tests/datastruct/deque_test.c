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
#include "ckit/datastruct/deque.h"

CK_TEST(init) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_deque *q;

    q = ck_deque_create(sizeof(int), ck_test_allocator_adapter(&test_allocator));
    CK_TEST_ASSERT_EQ(ck_deque_size(q), 0);

    ck_deque_destroy(q);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(peekback) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_deque *q;
    int first = 1;
    int second = 2;
    const int *out;

    q = ck_deque_create(sizeof(int), ck_test_allocator_adapter(&test_allocator));
    ck_deque_push(q, &first);
    ck_deque_push(q, &second);

    out = (const int *)ck_deque_peekback(q);
    CK_TEST_ASSERT_PTR_NOT_NULL(out);
    CK_TEST_ASSERT_EQ(*out, second);

    ck_deque_destroy(q);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(peekleft) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_deque *q;
    int first = 1;
    int second = 2;
    const int *out;

    q = ck_deque_create(sizeof(int), ck_test_allocator_adapter(&test_allocator));
    ck_deque_push(q, &first);
    ck_deque_push(q, &second);

    out = (const int *)ck_deque_peekleft(q);
    CK_TEST_ASSERT_PTR_NOT_NULL(out);
    CK_TEST_ASSERT_EQ(*out, first);

    ck_deque_destroy(q);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(popback) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_deque *q;
    int first = 1;
    int second = 2;
    int *out;

    q = ck_deque_create(sizeof(int), ck_test_allocator_adapter(&test_allocator));
    ck_deque_push(q, &first);
    ck_deque_push(q, &second);

    out = (int *)ck_deque_popback(q);
    CK_TEST_ASSERT_PTR_NOT_NULL(out);
    CK_TEST_ASSERT_EQ(*out, second);

    ck_deque_destroy(q);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(popleft) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_deque *q;
    int values[] = {10, 20};
    int *out;

    q = ck_deque_create(sizeof(int), ck_test_allocator_adapter(&test_allocator));
    CK_TEST_ASSERT_PTR_NULL(ck_deque_popleft(q));

    ck_deque_push(q, &values[0]);
    ck_deque_push(q, &values[1]);
    out = (int *)ck_deque_popleft(q);
    CK_TEST_ASSERT_PTR_NOT_NULL(out);
    CK_TEST_ASSERT_EQ(*out, values[0]);

    ck_deque_destroy(q);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(push) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_deque *q;
    int value = 42;

    q = ck_deque_create(sizeof(int), ck_test_allocator_adapter(&test_allocator));
    ck_deque_push(q, &value);

    const int *out = ck_deque_peekback(q);
    CK_TEST_ASSERT_EQ(ck_deque_size(q), 1);
    CK_TEST_ASSERT_PTR_NOT_NULL(out);
    CK_TEST_ASSERT_EQ(*out, value);

    ck_deque_destroy(q);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(pushfront) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_deque *q;
    int first = 1;
    int second = 2;
    int *out;

    q = ck_deque_create(sizeof(int), ck_test_allocator_adapter(&test_allocator));
    ck_deque_push(q, &first);
    ck_deque_pushfront(q, &second);

    out = (int *)ck_deque_popleft(q);
    CK_TEST_ASSERT_PTR_NOT_NULL(out);
    CK_TEST_ASSERT_EQ(*out, second);

    ck_deque_destroy(q);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST_MAIN(CK_TEST_CASE(init), CK_TEST_CASE(peekback), CK_TEST_CASE(peekleft),
             CK_TEST_CASE(popback), CK_TEST_CASE(popleft), CK_TEST_CASE(push),
             CK_TEST_CASE(pushfront))
