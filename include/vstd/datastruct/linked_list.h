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

#ifndef VSTD_LINKED_LIST_H
#define VSTD_LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>

#include "vstd/datastruct/iterator.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/utils.h"

#define VS_LINKED_LIST_FOR_EACH_NODE(item, list) \
    for (vs_iterator item##_vs_iter__ = vs_linked_list_get_iterator((list)); \
         ((item) = VS_ITER_NEXT_AS(vs_linked_list_node, &item##_vs_iter__)) != NULL;)

#define VS_LINKED_LIST_FOR_EACH_ENTRY(type, member, item, list) \
    for (vs_iterator item##_vs_iter__ = vs_linked_list_get_iterator((list)); \
         ((item) = \
              VS_CONTAINER_OF_CONST_OR_NULL(vs_iterator_next(&item##_vs_iter__), type, member)) \
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
 *  user object embeds vs_linked_list_node
 *
 * - push appends at tail
 * - pushfront prepends at head
 * - popleft removes from head
 * - linked list nodes and owning objects are caller-owned
 */

typedef struct vs_linked_list_node {
    struct vs_linked_list_node *next;
} vs_linked_list_node;

typedef struct vs_linked_list vs_linked_list;

/* Create an intrusive linked list. */
vs_linked_list *vs_linked_list_create(vs_allocator *allocator);

/* Append node at the tail. */
void vs_linked_list_push(vs_linked_list *list, vs_linked_list_node *node);

/* Prepend node at the head. */
void vs_linked_list_pushfront(vs_linked_list *list, vs_linked_list_node *node);

/* Remove and return the head node, or NULL when empty. */
vs_linked_list_node *vs_linked_list_popleft(vs_linked_list *list);

/* Remove and return the node after prev, or the head node when prev is NULL. */
vs_linked_list_node *vs_linked_list_remove_after(vs_linked_list *list, vs_linked_list_node *prev);

/* Return the number of stored elements. */
size_t vs_linked_list_size(const vs_linked_list *list);

/* Return the head node, or NULL when empty. */
vs_linked_list_node *vs_linked_list_head(const vs_linked_list *list);

/* Return an iterator over list nodes from head to tail. */
vs_iterator vs_linked_list_get_iterator(const vs_linked_list *list);

/* Release the linked-list handle. Nodes remain caller-owned. */
void vs_linked_list_destroy(vs_linked_list *list);

#endif
