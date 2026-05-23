#ifndef CKIT_DATASTRUCT_DOUBLY_LINKED_LIST_H
#define CKIT_DATASTRUCT_DOUBLY_LINKED_LIST_H

#include <stddef.h>
#include <stdint.h>

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
 *       user object embeds ckit_doubly_linked_list_node
 *
 * - push appends at tail
 * - pushfront prepends at head
 * - popleft removes from head
 * - popback removes from tail
 * - remove unlinks a known node in O(1)
 * - linked list nodes and owning objects are caller-owned
 */

#ifndef ckit_container_of
#define ckit_container_of(ptr, type, member) ((type *)((uint8_t *)(ptr) - offsetof(type, member)))
#endif

typedef struct ckit_doubly_linked_list_node {
    struct ckit_doubly_linked_list_node *prev;
    struct ckit_doubly_linked_list_node *next;
} ckit_doubly_linked_list_node;

typedef struct ckit_doubly_linked_list ckit_doubly_linked_list;

/* Create an intrusive doubly linked list. */
ckit_doubly_linked_list *ckit_doubly_linked_list_init(ckit_allocator *allocator);

/* Append node at the tail. */
void ckit_doubly_linked_list_push(ckit_doubly_linked_list *list,
                                  ckit_doubly_linked_list_node *node);

/* Prepend node at the head. */
void ckit_doubly_linked_list_pushfront(ckit_doubly_linked_list *list,
                                       ckit_doubly_linked_list_node *node);

/* Remove and return the head node, or NULL when empty. */
ckit_doubly_linked_list_node *ckit_doubly_linked_list_popleft(ckit_doubly_linked_list *list);

/* Remove and return the tail node, or NULL when empty. */
ckit_doubly_linked_list_node *ckit_doubly_linked_list_popback(ckit_doubly_linked_list *list);

/* Unlink node from list. */
void ckit_doubly_linked_list_remove(ckit_doubly_linked_list *list,
                                    ckit_doubly_linked_list_node *node);

/* Release the linked-list handle. Nodes remain caller-owned. */
void ckit_doubly_linked_list_free(ckit_doubly_linked_list *list);

/* Return the number of stored elements. */
size_t ckit_doubly_linked_list_size(const ckit_doubly_linked_list *list);

#endif
