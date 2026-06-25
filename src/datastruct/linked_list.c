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
#include "vstd/datastruct/iterator.h"
#include "vstd/datastruct/linked_list.h"
#include "vstd/memory/allocator.h"

struct vs_linked_list {
    size_t size;
    vs_linked_list_node *head;
    vs_linked_list_node *tail;
    vs_allocator *allocator;
};

vs_linked_list *vs_linked_list_create(vs_allocator *allocator) {
    vs_linked_list *list = vs_malloc(allocator, sizeof(vs_linked_list));
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->allocator = allocator;

    return list;
}

void vs_linked_list_push(vs_linked_list *list, vs_linked_list_node *node) {
    VSTD_ASSERT(list != NULL, "fatal: vs_linked_list_push invalid arguments");
    VSTD_ASSERT(node != NULL, "fatal: vs_linked_list_push invalid arguments");

    node->next = NULL;
    if (list->tail != NULL) {
        list->tail->next = node;
    } else {
        list->head = node;
    }

    list->tail = node;
    list->size += 1;
}

void vs_linked_list_pushfront(vs_linked_list *list, vs_linked_list_node *node) {
    VSTD_ASSERT(list != NULL, "fatal: vs_linked_list_pushfront invalid arguments");
    VSTD_ASSERT(node != NULL, "fatal: vs_linked_list_pushfront invalid arguments");

    node->next = list->head;
    list->head = node;

    if (list->tail == NULL) {
        list->tail = node;
    }

    list->size += 1;
}

vs_linked_list_node *vs_linked_list_popleft(vs_linked_list *list) {
    VSTD_ASSERT(list != NULL, "fatal: vs_linked_list_popleft invalid arguments");

    if (list->head == NULL) {
        return NULL;
    }

    vs_linked_list_node *old_head = list->head;
    list->head = old_head->next;
    old_head->next = NULL;

    if (list->head == NULL) {
        list->tail = NULL;
    }

    list->size -= 1;
    return old_head;
}

vs_linked_list_node *vs_linked_list_remove_after(vs_linked_list *list, vs_linked_list_node *prev) {
    VSTD_ASSERT(list != NULL, "fatal: vs_linked_list_remove_after invalid arguments");

    if (prev == NULL) {
        return vs_linked_list_popleft(list);
    }

    vs_linked_list_node *node = prev->next;
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

size_t vs_linked_list_size(const vs_linked_list *list) {
    VSTD_ASSERT(list != NULL, "fatal: vs_linked_list_size invalid arguments");

    return list->size;
}

vs_linked_list_node *vs_linked_list_head(const vs_linked_list *list) {
    VSTD_ASSERT(list != NULL, "fatal: vs_linked_list_head invalid arguments");

    return list->head;
}

typedef struct vs_linked_list_iterator_state {
    vs_linked_list_node *node;
} vs_linked_list_iterator_state;

_Static_assert(
    sizeof(vs_linked_list_iterator_state) <= VS_ITERATOR_STATE_SIZE,
    "vs_linked_list_iterator_state must fit in vs_iterator"
);

static const void *vs_linked_list_iterator_next(void *context) {
    VSTD_ASSERT(context != NULL, "fatal: vs_linked_list_iterator_next invalid arguments");

    vs_linked_list_iterator_state *iterator = context;
    vs_linked_list_node *node = iterator->node;
    if (node == NULL) {
        return NULL;
    }

    iterator->node = node->next;
    return node;
}

vs_iterator vs_linked_list_get_iterator(const vs_linked_list *list) {
    VSTD_ASSERT(list != NULL, "fatal: vs_linked_list_get_iterator invalid arguments");

    vs_iterator iter = vs_iterator_from_state(vs_linked_list_iterator_next);
    vs_linked_list_iterator_state *state = vs_iterator_state(&iter);
    state->node = list->head;
    vs_iterator_set_size_hint(&iter, list->size);
    return iter;
}

void vs_linked_list_destroy(vs_linked_list *list) {
    VSTD_ASSERT(list != NULL, "fatal: vs_linked_list_destroy invalid arguments");

    vs_allocator *allocator = list->allocator;
    vs_dealloc(allocator, list);
}
