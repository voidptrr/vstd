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
#include "vstd/datastruct/doubly_linked_list.h"
#include "vstd/datastruct/iterator.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"

struct vs_doubly_linked_list {
    size_t size;
    vs_doubly_linked_list_node *head;
    vs_doubly_linked_list_node *tail;
    vs_allocator *allocator;
};

typedef struct vs_doubly_linked_list_iterator_state {
    vs_doubly_linked_list_node *node;
} vs_doubly_linked_list_iterator_state;

_Static_assert(
    sizeof(vs_doubly_linked_list_iterator_state) <= VS_ITERATOR_STATE_SIZE,
    "vs_doubly_linked_list_iterator_state must fit in vs_iterator"
);

static const void *vs_doubly_linked_list_iterator_next(void *context) {
    VSTD_ASSERT(context != NULL, "fatal: vs_doubly_linked_list_iterator_next invalid arguments");

    vs_doubly_linked_list_iterator_state *iterator = context;
    vs_doubly_linked_list_node *node = iterator->node;
    if (node == NULL) {
        return NULL;
    }

    iterator->node = node->next;
    return node;
}

vs_status vs_doubly_linked_list_create(vs_allocator *allocator, vs_doubly_linked_list **out) {
    VSTD_ASSERT(out != NULL, "fatal: vs_doubly_linked_list_create invalid arguments");

    *out = NULL;

    vs_doubly_linked_list *list = NULL;
    vs_status status = vs_alloc(allocator, sizeof(vs_doubly_linked_list), (void **)&list);
    if (status != VS_STATUS_OK) {
        return status;
    }

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->allocator = allocator;

    *out = list;
    return VS_STATUS_OK;
}

void vs_doubly_linked_list_push(vs_doubly_linked_list *list, vs_doubly_linked_list_node *node) {
    VSTD_ASSERT(list != NULL, "fatal: vs_doubly_linked_list_push invalid arguments");
    VSTD_ASSERT(node != NULL, "fatal: vs_doubly_linked_list_push invalid arguments");

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

void vs_doubly_linked_list_pushfront(
    vs_doubly_linked_list *list,
    vs_doubly_linked_list_node *node
) {
    VSTD_ASSERT(list != NULL, "fatal: vs_doubly_linked_list_pushfront invalid arguments");
    VSTD_ASSERT(node != NULL, "fatal: vs_doubly_linked_list_pushfront invalid arguments");

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

void vs_doubly_linked_list_insert_after(
    vs_doubly_linked_list *list,
    vs_doubly_linked_list_node *after,
    vs_doubly_linked_list_node *node
) {
    VSTD_ASSERT(list != NULL, "fatal: vs_doubly_linked_list_insert_after invalid arguments");
    VSTD_ASSERT(node != NULL, "fatal: vs_doubly_linked_list_insert_after invalid arguments");

    if (after == NULL) {
        vs_doubly_linked_list_pushfront(list, node);
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

vs_doubly_linked_list_node *vs_doubly_linked_list_popleft(vs_doubly_linked_list *list) {
    VSTD_ASSERT(list != NULL, "fatal: vs_doubly_linked_list_popleft invalid arguments");

    if (list->head == NULL) {
        return NULL;
    }

    vs_doubly_linked_list_node *old_head = list->head;
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

vs_doubly_linked_list_node *vs_doubly_linked_list_popback(vs_doubly_linked_list *list) {
    VSTD_ASSERT(list != NULL, "fatal: vs_doubly_linked_list_popback invalid arguments");

    if (list->tail == NULL) {
        return NULL;
    }

    vs_doubly_linked_list_node *old_tail = list->tail;
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

void vs_doubly_linked_list_remove(vs_doubly_linked_list *list, vs_doubly_linked_list_node *node) {
    VSTD_ASSERT(list != NULL, "fatal: vs_doubly_linked_list_remove invalid arguments");
    VSTD_ASSERT(node != NULL, "fatal: vs_doubly_linked_list_remove invalid arguments");

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

size_t vs_doubly_linked_list_size(const vs_doubly_linked_list *list) {
    VSTD_ASSERT(list != NULL, "fatal: vs_doubly_linked_list_size invalid arguments");

    return list->size;
}

vs_doubly_linked_list_node *vs_doubly_linked_list_head(const vs_doubly_linked_list *list) {
    VSTD_ASSERT(list != NULL, "fatal: vs_doubly_linked_list_head invalid arguments");

    return list->head;
}

vs_doubly_linked_list_node *vs_doubly_linked_list_tail(const vs_doubly_linked_list *list) {
    VSTD_ASSERT(list != NULL, "fatal: vs_doubly_linked_list_tail invalid arguments");

    return list->tail;
}

vs_iterator vs_doubly_linked_list_get_iterator(const vs_doubly_linked_list *list) {
    VSTD_ASSERT(list != NULL, "fatal: vs_doubly_linked_list_get_iterator invalid arguments");

    vs_iterator iter = vs_iterator_from_state(vs_doubly_linked_list_iterator_next);
    vs_doubly_linked_list_iterator_state *state = vs_iterator_state(&iter);
    state->node = list->head;
    vs_iterator_set_size_hint(&iter, list->size);
    return iter;
}

void vs_doubly_linked_list_destroy(vs_doubly_linked_list *list) {
    VSTD_ASSERT(list != NULL, "fatal: vs_doubly_linked_list_destroy invalid arguments");

    vs_allocator *allocator = list->allocator;
    vs_dealloc(allocator, list);
}
