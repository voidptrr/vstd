#ifndef CKIT_DATASTRUCT_LINKED_LIST_H
#define CKIT_DATASTRUCT_LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/memory/allocators/allocator.h"

/*
 * Opaque generic singly linked list.
 *
 * Logical view:
 *
 *   head                                      tail
 *    |                                         |
 *    v                                         v
 *  +------+    +------+    +------+        +------+
 *  |  T   | -> |  T   | -> |  T   |  ...   |  T   | -> NULL
 *  +------+    +------+    +------+        +------+
 *
 * - push appends at tail
 * - pushfront prepends at head
 * - popleft removes from head
 */
typedef struct ckit_linked_list ckit_linked_list;

/* Create a linked list with element size elem_size. */
ckit_linked_list *ckit_linked_list_init(size_t elem_size, ckit_allocator *allocator);

/* Append one element by copying elem_size bytes from element. */
void ckit_linked_list_push(ckit_linked_list *list, const void *element);

/* Prepend one element by copying elem_size bytes from element. */
void ckit_linked_list_pushfront(ckit_linked_list *list, const void *element);

/* Remove and return the first element pointer, or NULL when empty. */
void *ckit_linked_list_popleft(ckit_linked_list *list);

/* Release owned nodes and the linked-list handle. */
void ckit_linked_list_free(ckit_linked_list *list);

/* Return the number of stored elements. */
size_t ckit_linked_list_size(const ckit_linked_list *list);

/* Return whether the linked list has zero elements. */
bool ckit_linked_list_is_empty(const ckit_linked_list *list);

#endif
