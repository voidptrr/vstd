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

#include "vstd/assert.h"
#include "vstd/datastruct/vector.h"
#include "vstd/memory/allocators/test_allocator.h"
#include "vstd/testing.h"

VS_TEST(init) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_vector *v;
    int value = 1;

    v = vs_vector_create(sizeof(int), vs_test_allocator_adapter(&test_allocator));
    VS_ASSERT_EQ(vs_vector_size(v), 0);

    vs_vector_push(v, &value);
    VS_ASSERT_EQ(vs_vector_size(v), 1);

    vs_vector_destroy(v);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(pop) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_vector *v;
    int first = 7;
    int second = 11;
    int *popped;

    v = vs_vector_create(sizeof(int), vs_test_allocator_adapter(&test_allocator));
    VS_ASSERT_PTR_NULL(vs_vector_pop(v));

    vs_vector_push(v, &first);
    vs_vector_push(v, &second);

    popped = (int *)vs_vector_pop(v);
    VS_ASSERT_PTR_NOT_NULL(popped);
    VS_ASSERT_EQ(*popped, second);

    popped = (int *)vs_vector_pop(v);
    VS_ASSERT_PTR_NOT_NULL(popped);
    VS_ASSERT_EQ(*popped, first);

    vs_vector_destroy(v);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(push_single_element) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_vector *v;
    int value = 42;

    v = vs_vector_create(sizeof(int), vs_test_allocator_adapter(&test_allocator));
    vs_vector_push(v, &value);

    VS_ASSERT_EQ(vs_vector_size(v), 1);
    VS_ASSERT_EQ(*(int *)vs_vector_get(v, 0), value);

    vs_vector_destroy(v);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(push_grows_storage) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_vector *v;

    v = vs_vector_create(sizeof(int), vs_test_allocator_adapter(&test_allocator));
    for (size_t i = 0; i < 17; i++) {
        int value = (int)i;
        vs_vector_push(v, &value);
    }

    VS_ASSERT_EQ(vs_vector_size(v), 17);

    vs_vector_destroy(v);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(push_preserves_existing_items_after_growth) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_vector *v;

    v = vs_vector_create(sizeof(int), vs_test_allocator_adapter(&test_allocator));
    for (size_t i = 0; i < 17; i++) {
        int value = (int)i;
        vs_vector_push(v, &value);
    }

    for (size_t i = 0; i < 17; i++) {
        VS_ASSERT_EQ(*(int *)vs_vector_get(v, i), (int)i);
    }

    vs_vector_destroy(v);
    VS_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST_MAIN(
    VS_TEST_CASE(init),
    VS_TEST_CASE(pop),
    VS_TEST_CASE(push_single_element),
    VS_TEST_CASE(push_grows_storage),
    VS_TEST_CASE(push_preserves_existing_items_after_growth)
)
