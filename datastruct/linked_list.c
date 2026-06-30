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

#include "k4c/allocators/allocator.h"
#include "k4c/assert.h"
#include "k4c/datastruct/iterator.h"
#include "k4c/datastruct/linked_list.h"
#include "k4c/error.h"

struct k4c_linked_list {
    size_t size;
    k4c_linked_list_node *head;
    k4c_linked_list_node *tail;
    k4c_allocator k4c_allocator;
};

typedef struct k4c_linked_list_iterator_state {
    k4c_linked_list_node *node;
} k4c_linked_list_iterator_state;

_Static_assert(
    sizeof(k4c_linked_list_iterator_state) <= K4C_ITERATOR_STATE_SIZE,
    "k4c_linked_list_iterator_state must fit in k4c_iterator"
);

static const void *k4c_linked_list_iterator_next(void *context) {
    K4C_ASSERT(context != NULL, "fatal: k4c_linked_list_iterator_next invalid arguments");

    k4c_linked_list_iterator_state *k4c_iterator = context;
    k4c_linked_list_node *node = k4c_iterator->node;
    if (node == NULL) {
        return NULL;
    }

    k4c_iterator->node = node->next;
    return node;
}

k4c_status k4c_linked_list_create(k4c_allocator *k4c_allocator, k4c_linked_list **out) {
    K4C_ASSERT(out != NULL, "fatal: k4c_linked_list_create invalid arguments");

    *out = NULL;

    k4c_linked_list *list = NULL;
    k4c_status st = k4c_alloc(k4c_allocator, sizeof(*list), (void **)&list);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->k4c_allocator = k4c_allocator_copy(k4c_allocator);

    *out = list;
    return K4C_STATUS_OK;
}

void k4c_linked_list_push(k4c_linked_list *list, k4c_linked_list_node *node) {
    K4C_ASSERT(list != NULL, "fatal: k4c_linked_list_push invalid arguments");
    K4C_ASSERT(node != NULL, "fatal: k4c_linked_list_push invalid arguments");

    node->next = NULL;
    if (list->tail != NULL) {
        list->tail->next = node;
    } else {
        list->head = node;
    }

    list->tail = node;
    list->size += 1;
}

void k4c_linked_list_pushfront(k4c_linked_list *list, k4c_linked_list_node *node) {
    K4C_ASSERT(list != NULL, "fatal: k4c_linked_list_pushfront invalid arguments");
    K4C_ASSERT(node != NULL, "fatal: k4c_linked_list_pushfront invalid arguments");

    node->next = list->head;
    list->head = node;

    if (list->tail == NULL) {
        list->tail = node;
    }

    list->size += 1;
}

k4c_linked_list_node *k4c_linked_list_popleft(k4c_linked_list *list) {
    K4C_ASSERT(list != NULL, "fatal: k4c_linked_list_popleft invalid arguments");

    if (list->head == NULL) {
        return NULL;
    }

    k4c_linked_list_node *old_head = list->head;
    list->head = old_head->next;
    old_head->next = NULL;

    if (list->head == NULL) {
        list->tail = NULL;
    }

    list->size -= 1;
    return old_head;
}

k4c_linked_list_node *k4c_linked_list_remove_after(
    k4c_linked_list *list,
    k4c_linked_list_node *prev
) {
    K4C_ASSERT(list != NULL, "fatal: k4c_linked_list_remove_after invalid arguments");

    if (prev == NULL) {
        return k4c_linked_list_popleft(list);
    }

    k4c_linked_list_node *node = prev->next;
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

size_t k4c_linked_list_size(const k4c_linked_list *list) {
    K4C_ASSERT(list != NULL, "fatal: k4c_linked_list_size invalid arguments");

    return list->size;
}

k4c_linked_list_node *k4c_linked_list_head(const k4c_linked_list *list) {
    K4C_ASSERT(list != NULL, "fatal: k4c_linked_list_head invalid arguments");

    return list->head;
}

k4c_iterator k4c_linked_list_get_iterator(const k4c_linked_list *list) {
    K4C_ASSERT(list != NULL, "fatal: k4c_linked_list_get_iterator invalid arguments");

    k4c_iterator iter = k4c_iterator_from_state(k4c_linked_list_iterator_next);
    k4c_linked_list_iterator_state *state = k4c_iterator_state(&iter);
    state->node = list->head;
    k4c_iterator_set_size_hint(&iter, list->size);
    return iter;
}

void k4c_linked_list_destroy(k4c_linked_list *list) {
    K4C_ASSERT(list != NULL, "fatal: k4c_linked_list_destroy invalid arguments");

    k4c_allocator *k4c_allocator = &list->k4c_allocator;
    k4c_dealloc(k4c_allocator, list);
}
