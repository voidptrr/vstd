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

#include "vstd/assert.h"
#include "vstd/ds/iterator.h"
#include "vstd/ds/linked_list.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"

struct linked_list {
    size_t size;
    linked_list_node *head;
    linked_list_node *tail;
    allocator *allocator;
};

typedef struct linked_list_iterator_state {
    linked_list_node *node;
} linked_list_iterator_state;

_Static_assert(
    sizeof(linked_list_iterator_state) <= ITERATOR_STATE_SIZE,
    "linked_list_iterator_state must fit in iterator"
);

static const void *linked_list_iterator_next(void *context) {
    ASSERT(context != NULL, "fatal: linked_list_iterator_next invalid arguments");

    linked_list_iterator_state *iterator = context;
    linked_list_node *node = iterator->node;
    if (node == NULL) {
        return NULL;
    }

    iterator->node = node->next;
    return node;
}

status linked_list_create(allocator *allocator, linked_list **out) {
    ASSERT(out != NULL, "fatal: linked_list_create invalid arguments");

    *out = NULL;

    linked_list *list = NULL;
    status st = alloc(allocator, sizeof(*list), (void **)&list);
    if (st != STATUS_OK) {
        return st;
    }

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->allocator = allocator;

    *out = list;
    return STATUS_OK;
}

void linked_list_push(linked_list *list, linked_list_node *node) {
    ASSERT(list != NULL, "fatal: linked_list_push invalid arguments");
    ASSERT(node != NULL, "fatal: linked_list_push invalid arguments");

    node->next = NULL;
    if (list->tail != NULL) {
        list->tail->next = node;
    } else {
        list->head = node;
    }

    list->tail = node;
    list->size += 1;
}

void linked_list_pushfront(linked_list *list, linked_list_node *node) {
    ASSERT(list != NULL, "fatal: linked_list_pushfront invalid arguments");
    ASSERT(node != NULL, "fatal: linked_list_pushfront invalid arguments");

    node->next = list->head;
    list->head = node;

    if (list->tail == NULL) {
        list->tail = node;
    }

    list->size += 1;
}

linked_list_node *linked_list_popleft(linked_list *list) {
    ASSERT(list != NULL, "fatal: linked_list_popleft invalid arguments");

    if (list->head == NULL) {
        return NULL;
    }

    linked_list_node *old_head = list->head;
    list->head = old_head->next;
    old_head->next = NULL;

    if (list->head == NULL) {
        list->tail = NULL;
    }

    list->size -= 1;
    return old_head;
}

linked_list_node *linked_list_remove_after(linked_list *list, linked_list_node *prev) {
    ASSERT(list != NULL, "fatal: linked_list_remove_after invalid arguments");

    if (prev == NULL) {
        return linked_list_popleft(list);
    }

    linked_list_node *node = prev->next;
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

size_t linked_list_size(const linked_list *list) {
    ASSERT(list != NULL, "fatal: linked_list_size invalid arguments");

    return list->size;
}

linked_list_node *linked_list_head(const linked_list *list) {
    ASSERT(list != NULL, "fatal: linked_list_head invalid arguments");

    return list->head;
}

iterator linked_list_get_iterator(const linked_list *list) {
    ASSERT(list != NULL, "fatal: linked_list_get_iterator invalid arguments");

    iterator iter = iterator_from_state(linked_list_iterator_next);
    linked_list_iterator_state *state = iterator_state(&iter);
    state->node = list->head;
    iterator_set_size_hint(&iter, list->size);
    return iter;
}

void linked_list_destroy(linked_list *list) {
    ASSERT(list != NULL, "fatal: linked_list_destroy invalid arguments");

    allocator *allocator = list->allocator;
    dealloc(allocator, list);
}
