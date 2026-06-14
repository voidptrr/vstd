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
#include "ckit/datastruct/doubly_linked_list.h"
#include "ckit/memory/allocators/allocator.h"

struct ck_doubly_linked_list {
    size_t size;
    ck_doubly_linked_list_node *head;
    ck_doubly_linked_list_node *tail;
    ck_allocator *allocator;
};

ck_doubly_linked_list *ck_doubly_linked_list_create(ck_allocator *allocator) {
    ck_doubly_linked_list *list = ck_malloc(allocator, sizeof(ck_doubly_linked_list));
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->allocator = allocator;

    return list;
}

void ck_doubly_linked_list_push(ck_doubly_linked_list *list, ck_doubly_linked_list_node *node) {
    CK_ASSERT(list != NULL, "fatal: ck_doubly_linked_list_push invalid arguments");
    CK_ASSERT(node != NULL, "fatal: ck_doubly_linked_list_push invalid arguments");

    node->prev = list->tail;
    node->next = NULL;

    if (list->tail != NULL) {
        list->tail->next = node;
    } else {
        list->head = node;
    }

    list->tail = node;
    list->size += 1;
}

void ck_doubly_linked_list_pushfront(ck_doubly_linked_list *list,
                                     ck_doubly_linked_list_node *node) {
    CK_ASSERT(list != NULL, "fatal: ck_doubly_linked_list_pushfront invalid arguments");
    CK_ASSERT(node != NULL, "fatal: ck_doubly_linked_list_pushfront invalid arguments");

    node->prev = NULL;
    node->next = list->head;

    if (list->head != NULL) {
        list->head->prev = node;
    } else {
        list->tail = node;
    }

    list->head = node;
    list->size += 1;
}

void ck_doubly_linked_list_insert_after(ck_doubly_linked_list *list,
                                        ck_doubly_linked_list_node *after,
                                        ck_doubly_linked_list_node *node) {
    CK_ASSERT(list != NULL, "fatal: ck_doubly_linked_list_insert_after invalid arguments");
    CK_ASSERT(node != NULL, "fatal: ck_doubly_linked_list_insert_after invalid arguments");

    if (after == NULL) {
        ck_doubly_linked_list_pushfront(list, node);
        return;
    }

    node->prev = after;
    node->next = after->next;

    if (after->next != NULL) {
        after->next->prev = node;
    } else {
        list->tail = node;
    }

    after->next = node;
    list->size += 1;
}

ck_doubly_linked_list_node *ck_doubly_linked_list_popleft(ck_doubly_linked_list *list) {
    CK_ASSERT(list != NULL, "fatal: ck_doubly_linked_list_popleft invalid arguments");

    if (list->head == NULL) {
        return NULL;
    }

    ck_doubly_linked_list_node *old_head = list->head;
    list->head = old_head->next;

    if (list->head != NULL) {
        list->head->prev = NULL;
    } else {
        list->tail = NULL;
    }

    old_head->prev = NULL;
    old_head->next = NULL;
    list->size -= 1;
    return old_head;
}

ck_doubly_linked_list_node *ck_doubly_linked_list_popback(ck_doubly_linked_list *list) {
    CK_ASSERT(list != NULL, "fatal: ck_doubly_linked_list_popback invalid arguments");

    if (list->tail == NULL) {
        return NULL;
    }

    ck_doubly_linked_list_node *old_tail = list->tail;
    list->tail = old_tail->prev;

    if (list->tail != NULL) {
        list->tail->next = NULL;
    } else {
        list->head = NULL;
    }

    old_tail->prev = NULL;
    old_tail->next = NULL;
    list->size -= 1;
    return old_tail;
}

void ck_doubly_linked_list_remove(ck_doubly_linked_list *list, ck_doubly_linked_list_node *node) {
    CK_ASSERT(list != NULL, "fatal: ck_doubly_linked_list_remove invalid arguments");
    CK_ASSERT(node != NULL, "fatal: ck_doubly_linked_list_remove invalid arguments");

    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }

    if (node->next != NULL) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }

    node->prev = NULL;
    node->next = NULL;
    list->size -= 1;
}

size_t ck_doubly_linked_list_size(const ck_doubly_linked_list *list) {
    CK_ASSERT(list != NULL, "fatal: ck_doubly_linked_list_size invalid arguments");

    return list->size;
}

ck_doubly_linked_list_node *ck_doubly_linked_list_head(const ck_doubly_linked_list *list) {
    CK_ASSERT(list != NULL, "fatal: ck_doubly_linked_list_head invalid arguments");

    return list->head;
}

ck_doubly_linked_list_node *ck_doubly_linked_list_tail(const ck_doubly_linked_list *list) {
    CK_ASSERT(list != NULL, "fatal: ck_doubly_linked_list_tail invalid arguments");

    return list->tail;
}

void ck_doubly_linked_list_destroy(ck_doubly_linked_list *list) {
    CK_ASSERT(list != NULL, "fatal: ck_doubly_linked_list_destroy invalid arguments");

    ck_allocator *allocator = list->allocator;
    ck_dealloc(allocator, list);
}
