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
#include "ckit/datastruct/linked_list.h"
#include "ckit/utils.h"

typedef struct test_item {
    int value;
    ck_linked_list_node node;
} test_item;

int main(void) {
    ck_linked_list *list;
    test_item first = {.value = 5};
    test_item second = {.value = 9};

    list = ck_linked_list_create(NULL);
    CK_TEST_ASSERT_PTR_NULL(ck_linked_list_popleft(list));

    ck_linked_list_push(list, &first.node);
    ck_linked_list_push(list, &second.node);

    ck_linked_list_node *out_node = ck_linked_list_popleft(list);
    CK_TEST_ASSERT_PTR_NOT_NULL(out_node);

    test_item *out = CK_CONTAINER_OF(out_node, test_item, node);
    CK_TEST_ASSERT_EQ(out->value, first.value);

    out_node = ck_linked_list_popleft(list);
    CK_TEST_ASSERT_PTR_NOT_NULL(out_node);

    out = CK_CONTAINER_OF(out_node, test_item, node);
    CK_TEST_ASSERT_EQ(out->value, second.value);

    ck_linked_list_destroy(list);
    return 0;
}
