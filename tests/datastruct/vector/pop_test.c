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
#include "ckit/datastruct/vector.h"

int main(void) {
    ck_vector *v;
    int first = 7;
    int second = 11;
    int *popped;

    v = ck_vector_create(sizeof(int), NULL);
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
    return 0;
}
