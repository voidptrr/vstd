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

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>

#include "vstd/ds/iterator.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/utils.h"

#define linked_list_for_each_node(item, list) \
    for (iterator item##_iter__ = linked_list_get_iterator((list)); \
         ((item) = ITER_NEXT_AS(linked_list_node, &item##_iter__)) != NULL;)

#define linked_list_for_each_entry(type, member, item, list) \
    for (iterator item##_iter__ = linked_list_get_iterator((list)); \
         ((item) = CONTAINER_OF_CONST_OR_NULL(iterator_next(&item##_iter__), type, member)) \
         != NULL;)

/*
 * Opaque intrusive singly linked list.
 *
 * Logical view:
 *
 *   head                                      tail
 *    |                                         |
 *    v                                         v
 *  +------+    +------+    +------+        +------+
 *  | node | -> | node | -> | node |  ...   | node | -> NULL
 *  +------+    +------+    +------+        +------+
 *     ^           ^           ^               ^
 *     |           |           |               |
 *  user object embeds linked_list_node
 *
 * - push appends at tail
 * - pushfront prepends at head
 * - popleft removes from head
 * - linked list nodes and owning objects are caller-owned
 */

typedef struct linked_list_node {
    struct linked_list_node *next;
} linked_list_node;

typedef struct linked_list linked_list;

/* Create an intrusive linked list. */
NODISCARD status linked_list_create(allocator *allocator, linked_list **out);

/* Append node at the tail. */
void linked_list_push(linked_list *list, linked_list_node *node);

/* Prepend node at the head. */
void linked_list_pushfront(linked_list *list, linked_list_node *node);

/* Remove and return the head node, or NULL when empty. */
linked_list_node *linked_list_popleft(linked_list *list);

/* Remove and return the node after prev, or the head node when prev is NULL. */
linked_list_node *linked_list_remove_after(linked_list *list, linked_list_node *prev);

/* Return the number of stored elements. */
size_t linked_list_size(const linked_list *list);

/* Return the head node, or NULL when empty. */
linked_list_node *linked_list_head(const linked_list *list);

/* Return an iterator over list nodes from head to tail. */
iterator linked_list_get_iterator(const linked_list *list);

/* Release the linked-list handle. Nodes remain caller-owned. */
void linked_list_destroy(linked_list *list);

#endif
