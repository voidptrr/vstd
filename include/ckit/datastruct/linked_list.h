#ifndef CKIT_DATASTRUCT_LINKED_LIST_H
#define CKIT_DATASTRUCT_LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ckit/memory/allocators/allocator.h"

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
 *  user object embeds ckit_linked_list_node
 *
 * - push appends at tail
 * - pushfront prepends at head
 * - popleft removes from head
 * - linked list nodes and owning objects are caller-owned
 */

#define ckit_container_of(ptr, type, member) ((type *)((uint8_t *)(ptr) - offsetof(type, member)))

typedef struct ckit_linked_list_node {
    struct ckit_linked_list_node *next;
} ckit_linked_list_node;

typedef struct ckit_linked_list ckit_linked_list;

/* Create an intrusive linked list. */
ckit_linked_list *ckit_linked_list_init(ckit_allocator *allocator);

/* Append node at the tail. */
void ckit_linked_list_push(ckit_linked_list *list, ckit_linked_list_node *node);

/* Prepend node at the head. */
void ckit_linked_list_pushfront(ckit_linked_list *list, ckit_linked_list_node *node);

/* Remove and return the head node, or NULL when empty. */
ckit_linked_list_node *ckit_linked_list_popleft(ckit_linked_list *list);

/* Release the linked-list handle. Nodes remain caller-owned. */
void ckit_linked_list_free(ckit_linked_list *list);

/* Return the number of stored elements. */
size_t ckit_linked_list_size(const ckit_linked_list *list);

#endif
