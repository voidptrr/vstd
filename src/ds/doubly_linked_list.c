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
#include "vstd/ds/doubly_linked_list.h"
#include "vstd/ds/iterator.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"

struct doubly_linked_list {
    size_t size;
    doubly_linked_list_node *head;
    doubly_linked_list_node *tail;
    allocator *allocator;
};

typedef struct doubly_linked_list_iterator_state {
    doubly_linked_list_node *node;
} doubly_linked_list_iterator_state;

_Static_assert(
    sizeof(doubly_linked_list_iterator_state) <= ITERATOR_STATE_SIZE,
    "doubly_linked_list_iterator_state must fit in iterator"
);

static const void *doubly_linked_list_iterator_next(void *context) {
    ASSERT(context != NULL, "fatal: doubly_linked_list_iterator_next invalid arguments");

    doubly_linked_list_iterator_state *iterator = context;
    doubly_linked_list_node *node = iterator->node;
    if (node == NULL) {
        return NULL;
    }

    iterator->node = node->next;
    return node;
}

status doubly_linked_list_create(allocator *allocator, doubly_linked_list **out) {
    ASSERT(out != NULL, "fatal: doubly_linked_list_create invalid arguments");

    *out = NULL;

    doubly_linked_list *list = NULL;
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

void doubly_linked_list_push(doubly_linked_list *list, doubly_linked_list_node *node) {
    ASSERT(list != NULL, "fatal: doubly_linked_list_push invalid arguments");
    ASSERT(node != NULL, "fatal: doubly_linked_list_push invalid arguments");

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

void doubly_linked_list_pushfront(doubly_linked_list *list, doubly_linked_list_node *node) {
    ASSERT(list != NULL, "fatal: doubly_linked_list_pushfront invalid arguments");
    ASSERT(node != NULL, "fatal: doubly_linked_list_pushfront invalid arguments");

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

void doubly_linked_list_insert_after(
    doubly_linked_list *list,
    doubly_linked_list_node *after,
    doubly_linked_list_node *node
) {
    ASSERT(list != NULL, "fatal: doubly_linked_list_insert_after invalid arguments");
    ASSERT(node != NULL, "fatal: doubly_linked_list_insert_after invalid arguments");

    if (after == NULL) {
        doubly_linked_list_pushfront(list, node);
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

doubly_linked_list_node *doubly_linked_list_popleft(doubly_linked_list *list) {
    ASSERT(list != NULL, "fatal: doubly_linked_list_popleft invalid arguments");

    if (list->head == NULL) {
        return NULL;
    }

    doubly_linked_list_node *old_head = list->head;
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

doubly_linked_list_node *doubly_linked_list_popback(doubly_linked_list *list) {
    ASSERT(list != NULL, "fatal: doubly_linked_list_popback invalid arguments");

    if (list->tail == NULL) {
        return NULL;
    }

    doubly_linked_list_node *old_tail = list->tail;
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

void doubly_linked_list_remove(doubly_linked_list *list, doubly_linked_list_node *node) {
    ASSERT(list != NULL, "fatal: doubly_linked_list_remove invalid arguments");
    ASSERT(node != NULL, "fatal: doubly_linked_list_remove invalid arguments");

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

size_t doubly_linked_list_size(const doubly_linked_list *list) {
    ASSERT(list != NULL, "fatal: doubly_linked_list_size invalid arguments");

    return list->size;
}

doubly_linked_list_node *doubly_linked_list_head(const doubly_linked_list *list) {
    ASSERT(list != NULL, "fatal: doubly_linked_list_head invalid arguments");

    return list->head;
}

doubly_linked_list_node *doubly_linked_list_tail(const doubly_linked_list *list) {
    ASSERT(list != NULL, "fatal: doubly_linked_list_tail invalid arguments");

    return list->tail;
}

iterator doubly_linked_list_get_iterator(const doubly_linked_list *list) {
    ASSERT(list != NULL, "fatal: doubly_linked_list_get_iterator invalid arguments");

    iterator iter = iterator_from_state(doubly_linked_list_iterator_next);
    doubly_linked_list_iterator_state *state = iterator_state(&iter);
    state->node = list->head;
    iterator_set_size_hint(&iter, list->size);
    return iter;
}

void doubly_linked_list_destroy(doubly_linked_list *list) {
    ASSERT(list != NULL, "fatal: doubly_linked_list_destroy invalid arguments");

    allocator *allocator = list->allocator;
    dealloc(allocator, list);
}
