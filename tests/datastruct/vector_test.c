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
#include "ckit/datastruct/vector.h"
#include <stddef.h>

CK_TEST(init) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_vector *v;
    int value = 1;

    v = ck_vector_create(sizeof(int), ck_test_allocator_adapter(&test_allocator));
    CK_TEST_ASSERT_EQ(ck_vector_size(v), 0);

    ck_vector_push(v, &value);
    CK_TEST_ASSERT_EQ(ck_vector_size(v), 1);

    ck_vector_destroy(v);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(pop) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_vector *v;
    int first = 7;
    int second = 11;
    int *popped;

    v = ck_vector_create(sizeof(int), ck_test_allocator_adapter(&test_allocator));
    CK_TEST_ASSERT_PTR_NULL(ck_vector_pop(v));

    ck_vector_push(v, &first);
    ck_vector_push(v, &second);

    popped = (int *)ck_vector_pop(v);
    CK_TEST_ASSERT_PTR_NOT_NULL(popped);
    CK_TEST_ASSERT_EQ(*popped, second);

    popped = (int *)ck_vector_pop(v);
    CK_TEST_ASSERT_PTR_NOT_NULL(popped);
    CK_TEST_ASSERT_EQ(*popped, first);

    ck_vector_destroy(v);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(push_single_element) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_vector *v;
    int value = 42;

    v = ck_vector_create(sizeof(int), ck_test_allocator_adapter(&test_allocator));
    ck_vector_push(v, &value);

    CK_TEST_ASSERT_EQ(ck_vector_size(v), 1);
    CK_TEST_ASSERT_EQ(*(int *)ck_vector_get(v, 0), value);

    ck_vector_destroy(v);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(push_grows_storage) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_vector *v;

    v = ck_vector_create(sizeof(int), ck_test_allocator_adapter(&test_allocator));
    for (size_t i = 0; i < 17; i++) {
        int value = (int)i;
        ck_vector_push(v, &value);
    }

    CK_TEST_ASSERT_EQ(ck_vector_size(v), 17);

    ck_vector_destroy(v);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST(push_preserves_existing_items_after_growth) {
    ck_test_allocator test_allocator;
    ck_test_allocator_init(&test_allocator);
    ck_vector *v;

    v = ck_vector_create(sizeof(int), ck_test_allocator_adapter(&test_allocator));
    for (size_t i = 0; i < 17; i++) {
        int value = (int)i;
        ck_vector_push(v, &value);
    }

    for (size_t i = 0; i < 17; i++) {
        CK_TEST_ASSERT_EQ(*(int *)ck_vector_get(v, i), (int)i);
    }

    ck_vector_destroy(v);
    CK_TEST_ASSERT(ck_test_allocator_is_clean(&test_allocator));
    return 0;
}

CK_TEST_MAIN(CK_TEST_CASE(init), CK_TEST_CASE(pop), CK_TEST_CASE(push_single_element),
             CK_TEST_CASE(push_grows_storage),
             CK_TEST_CASE(push_preserves_existing_items_after_growth))
