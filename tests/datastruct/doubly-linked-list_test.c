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

#include "vstd/datastruct/doubly_linked_list.h"
#include "vstd/datastruct/iterator.h"
#include "vstd/memory/test_allocator.h"
#include "vstd/memory/utils.h"
#include "vstd/testing.h"

typedef struct test_item {
    int value;
    vs_doubly_linked_list_node node;
} test_item;

VS_TEST(iterator_walks_nodes) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_doubly_linked_list *list;
    test_item first = {.value = 1};
    test_item second = {.value = 2};
    test_item third = {.value = 3};
    vs_iterator iter;
    const vs_doubly_linked_list_node *node;
    int expected = 1;

    list = vs_doubly_linked_list_create(vs_test_allocator_adapter(&test_allocator));
    vs_doubly_linked_list_push(list, &first.node);
    vs_doubly_linked_list_push(list, &second.node);
    vs_doubly_linked_list_push(list, &third.node);

    iter = vs_doubly_linked_list_get_iterator(list);
    while ((node = (const vs_doubly_linked_list_node *)vs_iterator_next(&iter)) != NULL) {
        const test_item *item = VS_CONTAINER_OF(node, test_item, node);
        if (vs_test_equal(item->value, expected) != 0) {
            return 1;
        }
        expected += 1;
    }
    if (vs_test_equal(expected, 4) != 0) {
        return 1;
    }

    vs_doubly_linked_list_destroy(list);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST_MAIN(VS_TEST_CASE(iterator_walks_nodes))
