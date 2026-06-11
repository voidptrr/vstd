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

#include <stdio.h>

#include "ckit/datastruct/linked_list.h"
#include "ckit/utils.h"

typedef struct test_item {
    int value;
    ck_linked_list_node node;
} test_item;

int main(void) {
    int status = 0;
    ck_linked_list *list;
    test_item first = {.value = 1};
    test_item second = {.value = 2};
    test_item third = {.value = 3};

    list = ck_linked_list_init(NULL);
    ck_linked_list_push(list, &first.node);
    ck_linked_list_push(list, &second.node);
    ck_linked_list_push(list, &third.node);

    ck_linked_list_node *removed = ck_linked_list_remove_after(list, &first.node);
    test_item *removed_item = CK_CONTAINER_OF(removed, test_item, node);
    if (removed_item->value != 2 || ck_linked_list_size(list) != 2) {
        fprintf(stderr, "remove_after should remove node after prev\n");
        status = 1;
        goto cleanup;
    }

    removed = ck_linked_list_remove_after(list, NULL);
    removed_item = CK_CONTAINER_OF(removed, test_item, node);
    if (removed_item->value != 1 || ck_linked_list_head(list) != &third.node) {
        fprintf(stderr, "remove_after with null prev should remove head\n");
        status = 1;
        goto cleanup;
    }

    removed = ck_linked_list_remove_after(list, &third.node);
    if (removed != NULL || ck_linked_list_size(list) != 1) {
        fprintf(stderr, "remove_after should return null when prev is tail\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_linked_list_deinit(list);
    return status;
}
