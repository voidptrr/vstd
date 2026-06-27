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

#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <stddef.h>

#include "vstd/ds/iterator.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/utils.h"

#define doubly_linked_list_for_each_node(item, list) \
    for (iterator item##_iter__ = doubly_linked_list_get_iterator((list)); \
         ((item) = ITER_NEXT_AS(doubly_linked_list_node, &item##_iter__)) != NULL;)

#define doubly_linked_list_for_each_entry(type, member, item, list) \
    for (iterator item##_iter__ = doubly_linked_list_get_iterator((list)); \
         ((item) = CONTAINER_OF_CONST_OR_NULL(iterator_next(&item##_iter__), type, member)) \
         != NULL;)

/*
 * Opaque intrusive doubly linked list.
 *
 * Logical view:
 *
 *                         head                         tail
 *                          |                            |
 *                          v                            v
 *  NULL <- +------+ <-> +------+ <-> +------+  ...  +------+ -> NULL
 *          | node |     | node |     | node |       | node |
 *          +------+     +------+     +------+       +------+
 *             ^            ^            ^              ^
 *             |            |            |              |
 *       user object embeds doubly_linked_list_node
 *
 * - push appends at tail
 * - pushfront prepends at head
 * - popleft removes from head
 * - popback removes from tail
 * - remove unlinks a known node in O(1)
 * - linked list nodes and owning objects are caller-owned
 */

typedef struct doubly_linked_list_node {
    struct doubly_linked_list_node *prev;
    struct doubly_linked_list_node *next;
} doubly_linked_list_node;

typedef struct doubly_linked_list doubly_linked_list;

/* Create an intrusive doubly linked list. */
NODISCARD status doubly_linked_list_create(allocator *allocator, doubly_linked_list **out);

/* Append node at the tail. */
void doubly_linked_list_push(doubly_linked_list *list, doubly_linked_list_node *node);

/* Prepend node at the head. */
void doubly_linked_list_pushfront(doubly_linked_list *list, doubly_linked_list_node *node);

/* Insert node after an existing node, or at the front when after is NULL. */
void doubly_linked_list_insert_after(
    doubly_linked_list *list,
    doubly_linked_list_node *after,
    doubly_linked_list_node *node
);

/* Remove and return the head node, or NULL when empty. */
doubly_linked_list_node *doubly_linked_list_popleft(doubly_linked_list *list);

/* Remove and return the tail node, or NULL when empty. */
doubly_linked_list_node *doubly_linked_list_popback(doubly_linked_list *list);

/* Unlink node from list. */
void doubly_linked_list_remove(doubly_linked_list *list, doubly_linked_list_node *node);

/* Return the number of stored elements. */
size_t doubly_linked_list_size(const doubly_linked_list *list);

/* Return the head node, or NULL when empty. */
doubly_linked_list_node *doubly_linked_list_head(const doubly_linked_list *list);

/* Return the tail node, or NULL when empty. */
doubly_linked_list_node *doubly_linked_list_tail(const doubly_linked_list *list);

/* Return an iterator over list nodes from head to tail. */
iterator doubly_linked_list_get_iterator(const doubly_linked_list *list);

/* Release the linked-list handle. Nodes remain caller-owned. */
void doubly_linked_list_destroy(doubly_linked_list *list);

#endif
