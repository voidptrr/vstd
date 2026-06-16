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

#include "vstd/datastruct/linked_list.h"
#include "vstd/memory/allocators/test_allocator.h"
#include "vstd/memory/utils.h"
#include "vstd/testing.h"

typedef struct test_item {
    int value;
    vs_linked_list_node node;
} test_item;

VS_TEST(head) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_linked_list *list;
    test_item first = {.value = 1};

    list = vs_linked_list_create(vs_test_allocator_adapter(&test_allocator));
    VS_TEST_ASSERT_PTR_NULL(vs_linked_list_head(list));

    vs_linked_list_push(list, &first.node);
    VS_TEST_ASSERT_PTR_EQ(vs_linked_list_head(list), &first.node);

    vs_linked_list_destroy(list);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(init) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_linked_list *list;
    list = vs_linked_list_create(vs_test_allocator_adapter(&test_allocator));

    VS_TEST_ASSERT_EQ(vs_linked_list_size(list), 0);

    vs_linked_list_destroy(list);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(popleft) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_linked_list *list;
    test_item first = {.value = 5};
    test_item second = {.value = 9};

    list = vs_linked_list_create(vs_test_allocator_adapter(&test_allocator));
    VS_TEST_ASSERT_PTR_NULL(vs_linked_list_popleft(list));

    vs_linked_list_push(list, &first.node);
    vs_linked_list_push(list, &second.node);

    vs_linked_list_node *out_node = vs_linked_list_popleft(list);
    VS_TEST_ASSERT_PTR_NOT_NULL(out_node);

    test_item *out = VS_CONTAINER_OF(out_node, test_item, node);
    VS_TEST_ASSERT_EQ(out->value, first.value);

    out_node = vs_linked_list_popleft(list);
    VS_TEST_ASSERT_PTR_NOT_NULL(out_node);

    out = VS_CONTAINER_OF(out_node, test_item, node);
    VS_TEST_ASSERT_EQ(out->value, second.value);

    vs_linked_list_destroy(list);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(push) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_linked_list *list;
    test_item first = {.value = 7};
    test_item second = {.value = 11};

    list = vs_linked_list_create(vs_test_allocator_adapter(&test_allocator));
    vs_linked_list_push(list, &first.node);
    vs_linked_list_push(list, &second.node);

    vs_linked_list_node *out_first_node = vs_linked_list_popleft(list);
    vs_linked_list_node *out_second_node = vs_linked_list_popleft(list);
    VS_TEST_ASSERT_PTR_NOT_NULL(out_first_node);
    VS_TEST_ASSERT_PTR_NOT_NULL(out_second_node);
    VS_TEST_ASSERT_EQ(vs_linked_list_size(list), 0);

    test_item *out_first = VS_CONTAINER_OF(out_first_node, test_item, node);
    test_item *out_second = VS_CONTAINER_OF(out_second_node, test_item, node);
    VS_TEST_ASSERT_EQ(out_first->value, first.value);
    VS_TEST_ASSERT_EQ(out_second->value, second.value);

    vs_linked_list_destroy(list);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(pushfront) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_linked_list *list;
    test_item first = {.value = 1};
    test_item second = {.value = 2};

    list = vs_linked_list_create(vs_test_allocator_adapter(&test_allocator));
    vs_linked_list_pushfront(list, &first.node);
    vs_linked_list_pushfront(list, &second.node);

    vs_linked_list_node *out_second_node = vs_linked_list_popleft(list);
    vs_linked_list_node *out_first_node = vs_linked_list_popleft(list);
    VS_TEST_ASSERT_PTR_NOT_NULL(out_second_node);
    VS_TEST_ASSERT_PTR_NOT_NULL(out_first_node);
    VS_TEST_ASSERT_EQ(vs_linked_list_size(list), 0);

    test_item *out_second = VS_CONTAINER_OF(out_second_node, test_item, node);
    test_item *out_first = VS_CONTAINER_OF(out_first_node, test_item, node);
    VS_TEST_ASSERT_EQ(out_second->value, second.value);
    VS_TEST_ASSERT_EQ(out_first->value, first.value);

    vs_linked_list_destroy(list);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST(remove_after) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_linked_list *list;
    test_item first = {.value = 1};
    test_item second = {.value = 2};
    test_item third = {.value = 3};

    list = vs_linked_list_create(vs_test_allocator_adapter(&test_allocator));
    vs_linked_list_push(list, &first.node);
    vs_linked_list_push(list, &second.node);
    vs_linked_list_push(list, &third.node);

    vs_linked_list_node *removed = vs_linked_list_remove_after(list, &first.node);
    test_item *removed_item = VS_CONTAINER_OF(removed, test_item, node);
    VS_TEST_ASSERT_EQ(removed_item->value, 2);
    VS_TEST_ASSERT_EQ(vs_linked_list_size(list), 2);

    removed = vs_linked_list_remove_after(list, NULL);
    removed_item = VS_CONTAINER_OF(removed, test_item, node);
    VS_TEST_ASSERT_EQ(removed_item->value, 1);
    VS_TEST_ASSERT_PTR_EQ(vs_linked_list_head(list), &third.node);

    removed = vs_linked_list_remove_after(list, &third.node);
    VS_TEST_ASSERT_PTR_NULL(removed);
    VS_TEST_ASSERT_EQ(vs_linked_list_size(list), 1);

    vs_linked_list_destroy(list);
    VS_TEST_ASSERT(vs_test_allocator_is_clean(&test_allocator));
    return 0;
}

VS_TEST_MAIN(
    VS_TEST_CASE(head),
    VS_TEST_CASE(init),
    VS_TEST_CASE(popleft),
    VS_TEST_CASE(push),
    VS_TEST_CASE(pushfront),
    VS_TEST_CASE(remove_after)
)
