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

#include "ckit/common/panic.h"
#include "ckit/datastruct/linked_list.h"
#include "ckit/memory/allocators/allocator.h"

struct ck_linked_list {
    size_t size;
    ck_linked_list_node *head;
    ck_linked_list_node *tail;
    ck_allocator *allocator;
};

ck_linked_list *ck_linked_list_create(ck_allocator *allocator) {
    ck_linked_list *list = ck_malloc(allocator, sizeof(ck_linked_list));
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->allocator = allocator;

    return list;
}

void ck_linked_list_push(ck_linked_list *list, ck_linked_list_node *node) {
    CK_ASSERT(list != NULL, "fatal: ck_linked_list_push invalid arguments");
    CK_ASSERT(node != NULL, "fatal: ck_linked_list_push invalid arguments");

    node->next = NULL;
    if (list->tail != NULL) {
        list->tail->next = node;
    } else {
        list->head = node;
    }

    list->tail = node;
    list->size += 1;
}

void ck_linked_list_pushfront(ck_linked_list *list, ck_linked_list_node *node) {
    CK_ASSERT(list != NULL, "fatal: ck_linked_list_pushfront invalid arguments");
    CK_ASSERT(node != NULL, "fatal: ck_linked_list_pushfront invalid arguments");

    node->next = list->head;
    list->head = node;

    if (list->tail == NULL) {
        list->tail = node;
    }

    list->size += 1;
}

ck_linked_list_node *ck_linked_list_popleft(ck_linked_list *list) {
    CK_ASSERT(list != NULL, "fatal: ck_linked_list_popleft invalid arguments");

    if (list->head == NULL) {
        return NULL;
    }

    ck_linked_list_node *old_head = list->head;
    list->head = old_head->next;
    old_head->next = NULL;

    if (list->head == NULL) {
        list->tail = NULL;
    }

    list->size -= 1;
    return old_head;
}

ck_linked_list_node *ck_linked_list_remove_after(ck_linked_list *list, ck_linked_list_node *prev) {
    CK_ASSERT(list != NULL, "fatal: ck_linked_list_remove_after invalid arguments");

    if (prev == NULL) {
        return ck_linked_list_popleft(list);
    }

    ck_linked_list_node *node = prev->next;
    if (node == NULL) {
        return NULL;
    }

    prev->next = node->next;
    if (list->tail == node) {
        list->tail = prev;
    }

    node->next = NULL;
    list->size -= 1;
    return node;
}

size_t ck_linked_list_size(const ck_linked_list *list) {
    CK_ASSERT(list != NULL, "fatal: ck_linked_list_size invalid arguments");

    return list->size;
}

ck_linked_list_node *ck_linked_list_head(const ck_linked_list *list) {
    CK_ASSERT(list != NULL, "fatal: ck_linked_list_head invalid arguments");

    return list->head;
}

void ck_linked_list_destroy(ck_linked_list *list) {
    CK_ASSERT(list != NULL, "fatal: ck_linked_list_destroy invalid arguments");

    ck_allocator *allocator = list->allocator;
    ck_dealloc(allocator, list);
}
