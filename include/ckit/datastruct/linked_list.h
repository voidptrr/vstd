#ifndef CK_DATASTRUCT_LINKED_LIST_H
#define CK_DATASTRUCT_LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/memory/allocators/allocator.h"
#include "ckit/utils.h"

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
 *  user object embeds ck_linked_list_node
 *
 * - push appends at tail
 * - pushfront prepends at head
 * - popleft removes from head
 * - linked list nodes and owning objects are caller-owned
 */

typedef struct ck_linked_list_node {
    struct ck_linked_list_node *next;
} ck_linked_list_node;

typedef struct ck_linked_list ck_linked_list;

/* Initialize an intrusive linked list. */
ck_linked_list *ck_linked_list_init(ck_allocator *allocator);

/* Append node at the tail. */
void ck_linked_list_push(ck_linked_list *list, ck_linked_list_node *node);

/* Prepend node at the head. */
void ck_linked_list_pushfront(ck_linked_list *list, ck_linked_list_node *node);

/* Remove and return the head node, or NULL when empty. */
ck_linked_list_node *ck_linked_list_popleft(ck_linked_list *list);

/* Remove and return the node after prev, or the head node when prev is NULL. */
ck_linked_list_node *ck_linked_list_remove_after(ck_linked_list *list, ck_linked_list_node *prev);

/* Return the number of stored elements. */
size_t ck_linked_list_size(const ck_linked_list *list);

/* Return the head node, or NULL when empty. */
ck_linked_list_node *ck_linked_list_head(const ck_linked_list *list);

/* Release the linked-list handle. Nodes remain caller-owned. */
void ck_linked_list_deinit(ck_linked_list *list);

#endif
