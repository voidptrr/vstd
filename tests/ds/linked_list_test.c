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

#include "vstd/ds/iterator.h"
#include "vstd/ds/linked_list.h"
#include "vstd/error.h"
#include "vstd/memory/test_allocator.h"
#include "vstd/memory/utils.h"
#include "vstd/testing.h"

typedef struct test_item {
    int value;
    linked_list_node node;
} test_item;

TEST(head) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    linked_list *list = NULL;
    if (test_equal(linked_list_create(allocator, &list), STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 1};

    if (test_null(linked_list_head(list)) != 0) {
        return 1;
    }

    linked_list_push(list, &first.node);
    if (test_equal_ptr(linked_list_head(list), &first.node) != 0) {
        return 1;
    }

    linked_list_destroy(list);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(init) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    linked_list *list = NULL;
    if (test_equal(linked_list_create(allocator, &list), STATUS_OK)) {
        return 1;
    }

    if (linked_list_size(list) != 0) {
        return 1;
    }

    linked_list_destroy(list);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(popleft) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    linked_list *list = NULL;
    if (test_equal(linked_list_create(allocator, &list), STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 5};
    test_item second = {.value = 9};

    if (test_null(linked_list_popleft(list)) != 0) {
        return 1;
    }

    linked_list_push(list, &first.node);
    linked_list_push(list, &second.node);

    linked_list_node *out_node = linked_list_popleft(list);
    if (test_not_null(out_node) != 0) {
        return 1;
    }

    test_item *out = CONTAINER_OF(out_node, test_item, node);
    if (test_equal(out->value, first.value) != 0) {
        return 1;
    }

    out_node = linked_list_popleft(list);
    if (test_not_null(out_node) != 0) {
        return 1;
    }

    out = CONTAINER_OF(out_node, test_item, node);
    if (test_equal(out->value, second.value) != 0) {
        return 1;
    }

    linked_list_destroy(list);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(push) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    linked_list *list = NULL;
    if (test_equal(linked_list_create(allocator, &list), STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 7};
    test_item second = {.value = 11};

    linked_list_push(list, &first.node);
    linked_list_push(list, &second.node);

    linked_list_node *out_first_node = linked_list_popleft(list);
    linked_list_node *out_second_node = linked_list_popleft(list);
    if (test_not_null(out_first_node) != 0) {
        return 1;
    }
    if (test_not_null(out_second_node) != 0) {
        return 1;
    }
    if (linked_list_size(list) != 0) {
        return 1;
    }

    test_item *out_first = CONTAINER_OF(out_first_node, test_item, node);
    test_item *out_second = CONTAINER_OF(out_second_node, test_item, node);
    if (test_equal(out_first->value, first.value) != 0) {
        return 1;
    }
    if (test_equal(out_second->value, second.value) != 0) {
        return 1;
    }

    linked_list_destroy(list);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(pushfront) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    linked_list *list = NULL;
    if (test_equal(linked_list_create(allocator, &list), STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 1};
    test_item second = {.value = 2};

    linked_list_pushfront(list, &first.node);
    linked_list_pushfront(list, &second.node);

    linked_list_node *out_second_node = linked_list_popleft(list);
    linked_list_node *out_first_node = linked_list_popleft(list);
    if (test_not_null(out_second_node) != 0) {
        return 1;
    }
    if (test_not_null(out_first_node) != 0) {
        return 1;
    }
    if (linked_list_size(list) != 0) {
        return 1;
    }

    test_item *out_second = CONTAINER_OF(out_second_node, test_item, node);
    test_item *out_first = CONTAINER_OF(out_first_node, test_item, node);
    if (test_equal(out_second->value, second.value) != 0) {
        return 1;
    }
    if (test_equal(out_first->value, first.value) != 0) {
        return 1;
    }

    linked_list_destroy(list);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(remove_after) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    linked_list *list = NULL;
    if (test_equal(linked_list_create(allocator, &list), STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 1};
    test_item second = {.value = 2};
    test_item third = {.value = 3};

    linked_list_push(list, &first.node);
    linked_list_push(list, &second.node);
    linked_list_push(list, &third.node);

    linked_list_node *removed = linked_list_remove_after(list, &first.node);
    test_item *removed_item = CONTAINER_OF(removed, test_item, node);
    if (test_equal(removed_item->value, 2) != 0) {
        return 1;
    }
    if (linked_list_size(list) != 2) {
        return 1;
    }

    removed = linked_list_remove_after(list, NULL);
    removed_item = CONTAINER_OF(removed, test_item, node);
    if (test_equal(removed_item->value, 1) != 0) {
        return 1;
    }
    if (test_equal_ptr(linked_list_head(list), &third.node) != 0) {
        return 1;
    }

    removed = linked_list_remove_after(list, &third.node);
    if (test_null(removed) != 0) {
        return 1;
    }
    if (linked_list_size(list) != 1) {
        return 1;
    }

    linked_list_destroy(list);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(iterator_walks_nodes) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    linked_list *list = NULL;
    if (test_equal(linked_list_create(allocator, &list), STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 1};
    test_item second = {.value = 2};
    test_item third = {.value = 3};
    const linked_list_node *node;
    int expected = 1;

    linked_list_push(list, &first.node);
    linked_list_push(list, &second.node);
    linked_list_push(list, &third.node);

    iterator iter = linked_list_get_iterator(list);
    while ((node = (const linked_list_node *)iterator_next(&iter)) != NULL) {
        const test_item *item = CONTAINER_OF(node, test_item, node);
        if (test_equal(item->value, expected) != 0) {
            return 1;
        }
        expected += 1;
    }
    if (test_equal(expected, 4) != 0) {
        return 1;
    }

    linked_list_destroy(list);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST_MAIN(
    TEST_CASE(head),
    TEST_CASE(init),
    TEST_CASE(popleft),
    TEST_CASE(push),
    TEST_CASE(pushfront),
    TEST_CASE(remove_after),
    TEST_CASE(iterator_walks_nodes)
)
