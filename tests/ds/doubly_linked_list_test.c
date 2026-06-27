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

#include "vstd/ds/doubly_linked_list.h"
#include "vstd/ds/iterator.h"
#include "vstd/error.h"
#include "vstd/memory/test_allocator.h"
#include "vstd/memory/utils.h"
#include "vstd/testing.h"

typedef struct test_item {
    int value;
    doubly_linked_list_node node;
} test_item;

TEST(init) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    doubly_linked_list *list = NULL;
    if (test_equal(doubly_linked_list_create(allocator, &list), STATUS_OK)) {
        return 1;
    }

    if (doubly_linked_list_size(list) != 0) {
        return 1;
    }
    if (test_null(doubly_linked_list_head(list)) != 0) {
        return 1;
    }
    if (test_null(doubly_linked_list_tail(list)) != 0) {
        return 1;
    }

    doubly_linked_list_destroy(list);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(push) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    doubly_linked_list *list = NULL;
    if (test_equal(doubly_linked_list_create(allocator, &list), STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 7};
    test_item second = {.value = 11};

    doubly_linked_list_push(list, &first.node);
    if (test_equal_ptr(doubly_linked_list_head(list), &first.node) != 0) {
        return 1;
    }
    if (test_equal_ptr(doubly_linked_list_tail(list), &first.node) != 0) {
        return 1;
    }
    if (test_null(first.node.prev) != 0) {
        return 1;
    }
    if (test_null(first.node.next) != 0) {
        return 1;
    }

    doubly_linked_list_push(list, &second.node);
    if (doubly_linked_list_size(list) != 2) {
        return 1;
    }
    if (test_equal_ptr(doubly_linked_list_head(list), &first.node) != 0) {
        return 1;
    }
    if (test_equal_ptr(doubly_linked_list_tail(list), &second.node) != 0) {
        return 1;
    }
    if (test_null(first.node.prev) != 0) {
        return 1;
    }
    if (test_equal_ptr(first.node.next, &second.node) != 0) {
        return 1;
    }
    if (test_equal_ptr(second.node.prev, &first.node) != 0) {
        return 1;
    }
    if (test_null(second.node.next) != 0) {
        return 1;
    }

    doubly_linked_list_destroy(list);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(pushfront) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    doubly_linked_list *list = NULL;
    if (test_equal(doubly_linked_list_create(allocator, &list), STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 1};
    test_item second = {.value = 2};

    doubly_linked_list_pushfront(list, &first.node);
    doubly_linked_list_pushfront(list, &second.node);

    if (doubly_linked_list_size(list) != 2) {
        return 1;
    }
    if (test_equal_ptr(doubly_linked_list_head(list), &second.node) != 0) {
        return 1;
    }
    if (test_equal_ptr(doubly_linked_list_tail(list), &first.node) != 0) {
        return 1;
    }
    if (test_null(second.node.prev) != 0) {
        return 1;
    }
    if (test_equal_ptr(second.node.next, &first.node) != 0) {
        return 1;
    }
    if (test_equal_ptr(first.node.prev, &second.node) != 0) {
        return 1;
    }
    if (test_null(first.node.next) != 0) {
        return 1;
    }

    doubly_linked_list_destroy(list);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(insert_after) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    doubly_linked_list *list = NULL;
    if (test_equal(doubly_linked_list_create(allocator, &list), STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 1};
    test_item second = {.value = 2};
    test_item third = {.value = 3};

    doubly_linked_list_insert_after(list, NULL, &second.node);
    if (test_equal_ptr(doubly_linked_list_head(list), &second.node) != 0) {
        return 1;
    }
    if (test_equal_ptr(doubly_linked_list_tail(list), &second.node) != 0) {
        return 1;
    }

    doubly_linked_list_insert_after(list, NULL, &first.node);
    doubly_linked_list_insert_after(list, &second.node, &third.node);

    if (doubly_linked_list_size(list) != 3) {
        return 1;
    }
    if (test_equal_ptr(doubly_linked_list_head(list), &first.node) != 0) {
        return 1;
    }
    if (test_equal_ptr(doubly_linked_list_tail(list), &third.node) != 0) {
        return 1;
    }
    if (test_null(first.node.prev) != 0) {
        return 1;
    }
    if (test_equal_ptr(first.node.next, &second.node) != 0) {
        return 1;
    }
    if (test_equal_ptr(second.node.prev, &first.node) != 0) {
        return 1;
    }
    if (test_equal_ptr(second.node.next, &third.node) != 0) {
        return 1;
    }
    if (test_equal_ptr(third.node.prev, &second.node) != 0) {
        return 1;
    }
    if (test_null(third.node.next) != 0) {
        return 1;
    }

    doubly_linked_list_destroy(list);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(popleft) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    doubly_linked_list *list = NULL;
    if (test_equal(doubly_linked_list_create(allocator, &list), STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 5};
    test_item second = {.value = 9};

    if (test_null(doubly_linked_list_popleft(list)) != 0) {
        return 1;
    }

    doubly_linked_list_push(list, &first.node);
    doubly_linked_list_push(list, &second.node);

    doubly_linked_list_node *out_node = doubly_linked_list_popleft(list);
    if (test_equal_ptr(out_node, &first.node) != 0) {
        return 1;
    }
    if (test_null(first.node.prev) != 0) {
        return 1;
    }
    if (test_null(first.node.next) != 0) {
        return 1;
    }
    if (test_equal_ptr(doubly_linked_list_head(list), &second.node) != 0) {
        return 1;
    }
    if (test_equal_ptr(doubly_linked_list_tail(list), &second.node) != 0) {
        return 1;
    }
    if (test_null(second.node.prev) != 0) {
        return 1;
    }

    out_node = doubly_linked_list_popleft(list);
    if (test_equal_ptr(out_node, &second.node) != 0) {
        return 1;
    }
    if (doubly_linked_list_size(list) != 0) {
        return 1;
    }
    if (test_null(doubly_linked_list_head(list)) != 0) {
        return 1;
    }
    if (test_null(doubly_linked_list_tail(list)) != 0) {
        return 1;
    }

    doubly_linked_list_destroy(list);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(popback) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    doubly_linked_list *list = NULL;
    if (test_equal(doubly_linked_list_create(allocator, &list), STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 5};
    test_item second = {.value = 9};

    if (test_null(doubly_linked_list_popback(list)) != 0) {
        return 1;
    }

    doubly_linked_list_push(list, &first.node);
    doubly_linked_list_push(list, &second.node);

    doubly_linked_list_node *out_node = doubly_linked_list_popback(list);
    if (test_equal_ptr(out_node, &second.node) != 0) {
        return 1;
    }
    if (test_null(second.node.prev) != 0) {
        return 1;
    }
    if (test_null(second.node.next) != 0) {
        return 1;
    }
    if (test_equal_ptr(doubly_linked_list_head(list), &first.node) != 0) {
        return 1;
    }
    if (test_equal_ptr(doubly_linked_list_tail(list), &first.node) != 0) {
        return 1;
    }
    if (test_null(first.node.next) != 0) {
        return 1;
    }

    out_node = doubly_linked_list_popback(list);
    if (test_equal_ptr(out_node, &first.node) != 0) {
        return 1;
    }
    if (doubly_linked_list_size(list) != 0) {
        return 1;
    }
    if (test_null(doubly_linked_list_head(list)) != 0) {
        return 1;
    }
    if (test_null(doubly_linked_list_tail(list)) != 0) {
        return 1;
    }

    doubly_linked_list_destroy(list);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(remove) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    doubly_linked_list *list = NULL;
    if (test_equal(doubly_linked_list_create(allocator, &list), STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 1};
    test_item second = {.value = 2};
    test_item third = {.value = 3};

    doubly_linked_list_push(list, &first.node);
    doubly_linked_list_push(list, &second.node);
    doubly_linked_list_push(list, &third.node);

    doubly_linked_list_remove(list, &second.node);
    if (doubly_linked_list_size(list) != 2) {
        return 1;
    }
    if (test_equal_ptr(first.node.next, &third.node) != 0) {
        return 1;
    }
    if (test_equal_ptr(third.node.prev, &first.node) != 0) {
        return 1;
    }
    if (test_null(second.node.prev) != 0) {
        return 1;
    }
    if (test_null(second.node.next) != 0) {
        return 1;
    }

    doubly_linked_list_remove(list, &first.node);
    if (test_equal_ptr(doubly_linked_list_head(list), &third.node) != 0) {
        return 1;
    }
    if (test_equal_ptr(doubly_linked_list_tail(list), &third.node) != 0) {
        return 1;
    }
    if (test_null(third.node.prev) != 0) {
        return 1;
    }

    doubly_linked_list_remove(list, &third.node);
    if (doubly_linked_list_size(list) != 0) {
        return 1;
    }
    if (test_null(doubly_linked_list_head(list)) != 0) {
        return 1;
    }
    if (test_null(doubly_linked_list_tail(list)) != 0) {
        return 1;
    }

    doubly_linked_list_destroy(list);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST(iterator_walks_nodes) {
    test_allocator test_allocator;
    allocator *allocator = test_allocator_init(&test_allocator);
    doubly_linked_list *list = NULL;
    if (test_equal(doubly_linked_list_create(allocator, &list), STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 1};
    test_item second = {.value = 2};
    test_item third = {.value = 3};
    const doubly_linked_list_node *node;
    int expected = 1;

    doubly_linked_list_push(list, &first.node);
    doubly_linked_list_push(list, &second.node);
    doubly_linked_list_push(list, &third.node);

    iterator iter = doubly_linked_list_get_iterator(list);
    while ((node = (const doubly_linked_list_node *)iterator_next(&iter)) != NULL) {
        const test_item *item = CONTAINER_OF(node, test_item, node);
        if (test_equal(item->value, expected) != 0) {
            return 1;
        }
        expected += 1;
    }
    if (test_equal(expected, 4) != 0) {
        return 1;
    }

    doubly_linked_list_destroy(list);
    if (test_equal(test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

TEST_MAIN(
    TEST_CASE(init),
    TEST_CASE(push),
    TEST_CASE(pushfront),
    TEST_CASE(insert_after),
    TEST_CASE(popleft),
    TEST_CASE(popback),
    TEST_CASE(remove),
    TEST_CASE(iterator_walks_nodes)
)
