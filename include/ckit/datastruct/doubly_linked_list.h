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

#ifndef CK_DATASTRUCT_DOUBLY_LINKED_LIST_H
#define CK_DATASTRUCT_DOUBLY_LINKED_LIST_H

#include <stddef.h>

#include "ckit/memory/allocators/allocator.h"

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
 *       user object embeds ck_doubly_linked_list_node
 *
 * - push appends at tail
 * - pushfront prepends at head
 * - popleft removes from head
 * - popback removes from tail
 * - remove unlinks a known node in O(1)
 * - linked list nodes and owning objects are caller-owned
 */

typedef struct ck_doubly_linked_list_node {
    struct ck_doubly_linked_list_node *prev;
    struct ck_doubly_linked_list_node *next;
} ck_doubly_linked_list_node;

typedef struct ck_doubly_linked_list ck_doubly_linked_list;

/* Create an intrusive doubly linked list. */
ck_doubly_linked_list *ck_doubly_linked_list_create(ck_allocator *allocator);

/* Append node at the tail. */
void ck_doubly_linked_list_push(ck_doubly_linked_list *list, ck_doubly_linked_list_node *node);

/* Prepend node at the head. */
void ck_doubly_linked_list_pushfront(ck_doubly_linked_list *list, ck_doubly_linked_list_node *node);

/* Insert node after an existing node, or at the front when after is NULL. */
void ck_doubly_linked_list_insert_after(ck_doubly_linked_list *list,
                                        ck_doubly_linked_list_node *after,
                                        ck_doubly_linked_list_node *node);

/* Remove and return the head node, or NULL when empty. */
ck_doubly_linked_list_node *ck_doubly_linked_list_popleft(ck_doubly_linked_list *list);

/* Remove and return the tail node, or NULL when empty. */
ck_doubly_linked_list_node *ck_doubly_linked_list_popback(ck_doubly_linked_list *list);

/* Unlink node from list. */
void ck_doubly_linked_list_remove(ck_doubly_linked_list *list, ck_doubly_linked_list_node *node);

/* Return the number of stored elements. */
size_t ck_doubly_linked_list_size(const ck_doubly_linked_list *list);

/* Return the head node, or NULL when empty. */
ck_doubly_linked_list_node *ck_doubly_linked_list_head(const ck_doubly_linked_list *list);

/* Return the tail node, or NULL when empty. */
ck_doubly_linked_list_node *ck_doubly_linked_list_tail(const ck_doubly_linked_list *list);

/* Release the linked-list handle. Nodes remain caller-owned. */
void ck_doubly_linked_list_destroy(ck_doubly_linked_list *list);

#endif
