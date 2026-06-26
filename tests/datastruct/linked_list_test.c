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

#include "vstd/datastruct/iterator.h"
#include "vstd/datastruct/linked_list.h"
#include "vstd/memory/test_allocator.h"
#include "vstd/memory/utils.h"
#include "vstd/testing.h"

typedef struct test_item {
    int value;
    vs_linked_list_node node;
} test_item;

VS_TEST(head) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_linked_list *list = vs_linked_list_create(allocator);
    test_item first = {.value = 1};

    if (vs_test_null(vs_linked_list_head(list)) != 0) {
        return 1;
    }

    vs_linked_list_push(list, &first.node);
    if (vs_test_equal_ptr(vs_linked_list_head(list), &first.node) != 0) {
        return 1;
    }

    vs_linked_list_destroy(list);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(init) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_linked_list *list = vs_linked_list_create(allocator);

    if (vs_linked_list_size(list) != 0) {
        return 1;
    }

    vs_linked_list_destroy(list);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(popleft) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_linked_list *list = vs_linked_list_create(allocator);
    test_item first = {.value = 5};
    test_item second = {.value = 9};

    if (vs_test_null(vs_linked_list_popleft(list)) != 0) {
        return 1;
    }

    vs_linked_list_push(list, &first.node);
    vs_linked_list_push(list, &second.node);

    vs_linked_list_node *out_node = vs_linked_list_popleft(list);
    if (vs_test_not_null(out_node) != 0) {
        return 1;
    }

    test_item *out = VS_CONTAINER_OF(out_node, test_item, node);
    if (vs_test_equal(out->value, first.value) != 0) {
        return 1;
    }

    out_node = vs_linked_list_popleft(list);
    if (vs_test_not_null(out_node) != 0) {
        return 1;
    }

    out = VS_CONTAINER_OF(out_node, test_item, node);
    if (vs_test_equal(out->value, second.value) != 0) {
        return 1;
    }

    vs_linked_list_destroy(list);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(push) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_linked_list *list = vs_linked_list_create(allocator);
    test_item first = {.value = 7};
    test_item second = {.value = 11};

    vs_linked_list_push(list, &first.node);
    vs_linked_list_push(list, &second.node);

    vs_linked_list_node *out_first_node = vs_linked_list_popleft(list);
    vs_linked_list_node *out_second_node = vs_linked_list_popleft(list);
    if (vs_test_not_null(out_first_node) != 0) {
        return 1;
    }
    if (vs_test_not_null(out_second_node) != 0) {
        return 1;
    }
    if (vs_linked_list_size(list) != 0) {
        return 1;
    }

    test_item *out_first = VS_CONTAINER_OF(out_first_node, test_item, node);
    test_item *out_second = VS_CONTAINER_OF(out_second_node, test_item, node);
    if (vs_test_equal(out_first->value, first.value) != 0) {
        return 1;
    }
    if (vs_test_equal(out_second->value, second.value) != 0) {
        return 1;
    }

    vs_linked_list_destroy(list);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(pushfront) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_linked_list *list = vs_linked_list_create(allocator);
    test_item first = {.value = 1};
    test_item second = {.value = 2};

    vs_linked_list_pushfront(list, &first.node);
    vs_linked_list_pushfront(list, &second.node);

    vs_linked_list_node *out_second_node = vs_linked_list_popleft(list);
    vs_linked_list_node *out_first_node = vs_linked_list_popleft(list);
    if (vs_test_not_null(out_second_node) != 0) {
        return 1;
    }
    if (vs_test_not_null(out_first_node) != 0) {
        return 1;
    }
    if (vs_linked_list_size(list) != 0) {
        return 1;
    }

    test_item *out_second = VS_CONTAINER_OF(out_second_node, test_item, node);
    test_item *out_first = VS_CONTAINER_OF(out_first_node, test_item, node);
    if (vs_test_equal(out_second->value, second.value) != 0) {
        return 1;
    }
    if (vs_test_equal(out_first->value, first.value) != 0) {
        return 1;
    }

    vs_linked_list_destroy(list);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(remove_after) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_linked_list *list = vs_linked_list_create(allocator);
    test_item first = {.value = 1};
    test_item second = {.value = 2};
    test_item third = {.value = 3};

    vs_linked_list_push(list, &first.node);
    vs_linked_list_push(list, &second.node);
    vs_linked_list_push(list, &third.node);

    vs_linked_list_node *removed = vs_linked_list_remove_after(list, &first.node);
    test_item *removed_item = VS_CONTAINER_OF(removed, test_item, node);
    if (vs_test_equal(removed_item->value, 2) != 0) {
        return 1;
    }
    if (vs_linked_list_size(list) != 2) {
        return 1;
    }

    removed = vs_linked_list_remove_after(list, NULL);
    removed_item = VS_CONTAINER_OF(removed, test_item, node);
    if (vs_test_equal(removed_item->value, 1) != 0) {
        return 1;
    }
    if (vs_test_equal_ptr(vs_linked_list_head(list), &third.node) != 0) {
        return 1;
    }

    removed = vs_linked_list_remove_after(list, &third.node);
    if (vs_test_null(removed) != 0) {
        return 1;
    }
    if (vs_linked_list_size(list) != 1) {
        return 1;
    }

    vs_linked_list_destroy(list);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(iterator_walks_nodes) {
    vs_test_allocator test_allocator;
    vs_allocator *allocator = vs_test_allocator_init(&test_allocator);
    vs_linked_list *list = vs_linked_list_create(allocator);
    test_item first = {.value = 1};
    test_item second = {.value = 2};
    test_item third = {.value = 3};
    const vs_linked_list_node *node;
    int expected = 1;

    vs_linked_list_push(list, &first.node);
    vs_linked_list_push(list, &second.node);
    vs_linked_list_push(list, &third.node);

    vs_iterator iter = vs_linked_list_get_iterator(list);
    while ((node = (const vs_linked_list_node *)vs_iterator_next(&iter)) != NULL) {
        const test_item *item = VS_CONTAINER_OF(node, test_item, node);
        if (vs_test_equal(item->value, expected) != 0) {
            return 1;
        }
        expected += 1;
    }
    if (vs_test_equal(expected, 4) != 0) {
        return 1;
    }

    vs_linked_list_destroy(list);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST_MAIN(
    VS_TEST_CASE(head),
    VS_TEST_CASE(init),
    VS_TEST_CASE(popleft),
    VS_TEST_CASE(push),
    VS_TEST_CASE(pushfront),
    VS_TEST_CASE(remove_after),
    VS_TEST_CASE(iterator_walks_nodes)
)
