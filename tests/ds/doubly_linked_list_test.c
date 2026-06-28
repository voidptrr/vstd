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

#include "k4c/ds/doubly_linked_list.h"
#include "k4c/ds/iterator.h"
#include "k4c/error.h"
#include "k4c/memory/allocator.h"
#include "k4c/memory/test_allocator.h"
#include "k4c/memory/utils.h"
#include "k4c/testing.h"

typedef struct test_item {
    int value;
    k4c_doubly_linked_list_node node;
} test_item;

K4C_TEST(init) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator *k4c_allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_doubly_linked_list *list = NULL;
    if (k4c_test_equal(k4c_doubly_linked_list_create(k4c_allocator, &list), K4C_STATUS_OK)) {
        return 1;
    }

    if (k4c_doubly_linked_list_size(list) != 0) {
        return 1;
    }
    if (k4c_test_null(k4c_doubly_linked_list_head(list)) != 0) {
        return 1;
    }
    if (k4c_test_null(k4c_doubly_linked_list_tail(list)) != 0) {
        return 1;
    }

    k4c_doubly_linked_list_destroy(list);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(push) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator *k4c_allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_doubly_linked_list *list = NULL;
    if (k4c_test_equal(k4c_doubly_linked_list_create(k4c_allocator, &list), K4C_STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 7};
    test_item second = {.value = 11};

    k4c_doubly_linked_list_push(list, &first.node);
    if (k4c_test_equal_ptr(k4c_doubly_linked_list_head(list), &first.node) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(k4c_doubly_linked_list_tail(list), &first.node) != 0) {
        return 1;
    }
    if (k4c_test_null(first.node.prev) != 0) {
        return 1;
    }
    if (k4c_test_null(first.node.next) != 0) {
        return 1;
    }

    k4c_doubly_linked_list_push(list, &second.node);
    if (k4c_doubly_linked_list_size(list) != 2) {
        return 1;
    }
    if (k4c_test_equal_ptr(k4c_doubly_linked_list_head(list), &first.node) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(k4c_doubly_linked_list_tail(list), &second.node) != 0) {
        return 1;
    }
    if (k4c_test_null(first.node.prev) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(first.node.next, &second.node) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(second.node.prev, &first.node) != 0) {
        return 1;
    }
    if (k4c_test_null(second.node.next) != 0) {
        return 1;
    }

    k4c_doubly_linked_list_destroy(list);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(pushfront) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator *k4c_allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_doubly_linked_list *list = NULL;
    if (k4c_test_equal(k4c_doubly_linked_list_create(k4c_allocator, &list), K4C_STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 1};
    test_item second = {.value = 2};

    k4c_doubly_linked_list_pushfront(list, &first.node);
    k4c_doubly_linked_list_pushfront(list, &second.node);

    if (k4c_doubly_linked_list_size(list) != 2) {
        return 1;
    }
    if (k4c_test_equal_ptr(k4c_doubly_linked_list_head(list), &second.node) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(k4c_doubly_linked_list_tail(list), &first.node) != 0) {
        return 1;
    }
    if (k4c_test_null(second.node.prev) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(second.node.next, &first.node) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(first.node.prev, &second.node) != 0) {
        return 1;
    }
    if (k4c_test_null(first.node.next) != 0) {
        return 1;
    }

    k4c_doubly_linked_list_destroy(list);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(insert_after) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator *k4c_allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_doubly_linked_list *list = NULL;
    if (k4c_test_equal(k4c_doubly_linked_list_create(k4c_allocator, &list), K4C_STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 1};
    test_item second = {.value = 2};
    test_item third = {.value = 3};

    k4c_doubly_linked_list_insert_after(list, NULL, &second.node);
    if (k4c_test_equal_ptr(k4c_doubly_linked_list_head(list), &second.node) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(k4c_doubly_linked_list_tail(list), &second.node) != 0) {
        return 1;
    }

    k4c_doubly_linked_list_insert_after(list, NULL, &first.node);
    k4c_doubly_linked_list_insert_after(list, &second.node, &third.node);

    if (k4c_doubly_linked_list_size(list) != 3) {
        return 1;
    }
    if (k4c_test_equal_ptr(k4c_doubly_linked_list_head(list), &first.node) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(k4c_doubly_linked_list_tail(list), &third.node) != 0) {
        return 1;
    }
    if (k4c_test_null(first.node.prev) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(first.node.next, &second.node) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(second.node.prev, &first.node) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(second.node.next, &third.node) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(third.node.prev, &second.node) != 0) {
        return 1;
    }
    if (k4c_test_null(third.node.next) != 0) {
        return 1;
    }

    k4c_doubly_linked_list_destroy(list);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(popleft) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator *k4c_allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_doubly_linked_list *list = NULL;
    if (k4c_test_equal(k4c_doubly_linked_list_create(k4c_allocator, &list), K4C_STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 5};
    test_item second = {.value = 9};

    if (k4c_test_null(k4c_doubly_linked_list_popleft(list)) != 0) {
        return 1;
    }

    k4c_doubly_linked_list_push(list, &first.node);
    k4c_doubly_linked_list_push(list, &second.node);

    k4c_doubly_linked_list_node *out_node = k4c_doubly_linked_list_popleft(list);
    if (k4c_test_equal_ptr(out_node, &first.node) != 0) {
        return 1;
    }
    if (k4c_test_null(first.node.prev) != 0) {
        return 1;
    }
    if (k4c_test_null(first.node.next) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(k4c_doubly_linked_list_head(list), &second.node) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(k4c_doubly_linked_list_tail(list), &second.node) != 0) {
        return 1;
    }
    if (k4c_test_null(second.node.prev) != 0) {
        return 1;
    }

    out_node = k4c_doubly_linked_list_popleft(list);
    if (k4c_test_equal_ptr(out_node, &second.node) != 0) {
        return 1;
    }
    if (k4c_doubly_linked_list_size(list) != 0) {
        return 1;
    }
    if (k4c_test_null(k4c_doubly_linked_list_head(list)) != 0) {
        return 1;
    }
    if (k4c_test_null(k4c_doubly_linked_list_tail(list)) != 0) {
        return 1;
    }

    k4c_doubly_linked_list_destroy(list);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(popback) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator *k4c_allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_doubly_linked_list *list = NULL;
    if (k4c_test_equal(k4c_doubly_linked_list_create(k4c_allocator, &list), K4C_STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 5};
    test_item second = {.value = 9};

    if (k4c_test_null(k4c_doubly_linked_list_popback(list)) != 0) {
        return 1;
    }

    k4c_doubly_linked_list_push(list, &first.node);
    k4c_doubly_linked_list_push(list, &second.node);

    k4c_doubly_linked_list_node *out_node = k4c_doubly_linked_list_popback(list);
    if (k4c_test_equal_ptr(out_node, &second.node) != 0) {
        return 1;
    }
    if (k4c_test_null(second.node.prev) != 0) {
        return 1;
    }
    if (k4c_test_null(second.node.next) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(k4c_doubly_linked_list_head(list), &first.node) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(k4c_doubly_linked_list_tail(list), &first.node) != 0) {
        return 1;
    }
    if (k4c_test_null(first.node.next) != 0) {
        return 1;
    }

    out_node = k4c_doubly_linked_list_popback(list);
    if (k4c_test_equal_ptr(out_node, &first.node) != 0) {
        return 1;
    }
    if (k4c_doubly_linked_list_size(list) != 0) {
        return 1;
    }
    if (k4c_test_null(k4c_doubly_linked_list_head(list)) != 0) {
        return 1;
    }
    if (k4c_test_null(k4c_doubly_linked_list_tail(list)) != 0) {
        return 1;
    }

    k4c_doubly_linked_list_destroy(list);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(remove) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator *k4c_allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_doubly_linked_list *list = NULL;
    if (k4c_test_equal(k4c_doubly_linked_list_create(k4c_allocator, &list), K4C_STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 1};
    test_item second = {.value = 2};
    test_item third = {.value = 3};

    k4c_doubly_linked_list_push(list, &first.node);
    k4c_doubly_linked_list_push(list, &second.node);
    k4c_doubly_linked_list_push(list, &third.node);

    k4c_doubly_linked_list_remove(list, &second.node);
    if (k4c_doubly_linked_list_size(list) != 2) {
        return 1;
    }
    if (k4c_test_equal_ptr(first.node.next, &third.node) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(third.node.prev, &first.node) != 0) {
        return 1;
    }
    if (k4c_test_null(second.node.prev) != 0) {
        return 1;
    }
    if (k4c_test_null(second.node.next) != 0) {
        return 1;
    }

    k4c_doubly_linked_list_remove(list, &first.node);
    if (k4c_test_equal_ptr(k4c_doubly_linked_list_head(list), &third.node) != 0) {
        return 1;
    }
    if (k4c_test_equal_ptr(k4c_doubly_linked_list_tail(list), &third.node) != 0) {
        return 1;
    }
    if (k4c_test_null(third.node.prev) != 0) {
        return 1;
    }

    k4c_doubly_linked_list_remove(list, &third.node);
    if (k4c_doubly_linked_list_size(list) != 0) {
        return 1;
    }
    if (k4c_test_null(k4c_doubly_linked_list_head(list)) != 0) {
        return 1;
    }
    if (k4c_test_null(k4c_doubly_linked_list_tail(list)) != 0) {
        return 1;
    }

    k4c_doubly_linked_list_destroy(list);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(iterator_walks_nodes) {
    k4c_test_allocator k4c_test_allocator;
    k4c_allocator *k4c_allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_doubly_linked_list *list = NULL;
    if (k4c_test_equal(k4c_doubly_linked_list_create(k4c_allocator, &list), K4C_STATUS_OK)) {
        return 1;
    }
    test_item first = {.value = 1};
    test_item second = {.value = 2};
    test_item third = {.value = 3};
    const k4c_doubly_linked_list_node *node;
    int expected = 1;

    k4c_doubly_linked_list_push(list, &first.node);
    k4c_doubly_linked_list_push(list, &second.node);
    k4c_doubly_linked_list_push(list, &third.node);

    k4c_iterator iter = k4c_doubly_linked_list_get_iterator(list);
    while ((node = (const k4c_doubly_linked_list_node *)k4c_iterator_next(&iter)) != NULL) {
        const test_item *item = K4C_CONTAINER_OF(node, test_item, node);
        if (k4c_test_equal(item->value, expected) != 0) {
            return 1;
        }
        expected += 1;
    }
    if (k4c_test_equal(expected, 4) != 0) {
        return 1;
    }

    k4c_doubly_linked_list_destroy(list);
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST_MAIN(
    K4C_TEST_CASE(init),
    K4C_TEST_CASE(push),
    K4C_TEST_CASE(pushfront),
    K4C_TEST_CASE(insert_after),
    K4C_TEST_CASE(popleft),
    K4C_TEST_CASE(popback),
    K4C_TEST_CASE(remove),
    K4C_TEST_CASE(iterator_walks_nodes)
)
