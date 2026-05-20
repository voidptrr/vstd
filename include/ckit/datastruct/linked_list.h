#ifndef CKIT_DATASTRUCT_LINKED_LIST_H
#define CKIT_DATASTRUCT_LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/mem/allocators/allocator.h"

/*
 * Generic singly linked list.
 * +----------------------+----------------------+----------------------+
 * | size (size_t)        | elem_size (size_t)   | head (node *)        |
 * +----------------------+----------------------+----------------------+
 * | tail (node *)        |                      |                      |
 * +----------------------+----------------------+----------------------+
 */

typedef struct ckit_linked_list_node {
    struct ckit_linked_list_node *next;
    void *data;
} ckit_linked_list_node;

typedef struct ckit_linked_list {
    size_t size;
    size_t elem_size;
    ckit_linked_list_node *head;
    ckit_linked_list_node *tail;
    ckit_allocator *allocator;
} ckit_linked_list;

/* Initialize a linked list with element size elem_size. */
void ckit_linked_list_init(ckit_linked_list *list, size_t elem_size, ckit_allocator *allocator);

/* Append one element by copying elem_size bytes from element. */
void ckit_linked_list_push(ckit_linked_list *list, const void *element);

/* Prepend one element by copying elem_size bytes from element. */
void ckit_linked_list_pushfront(ckit_linked_list *list, const void *element);

/* Remove and return the first element pointer, or NULL when empty. */
void *ckit_linked_list_popleft(ckit_linked_list *list);

/* Release owned nodes and reset list to an empty state. */
void ckit_linked_list_free(ckit_linked_list *list);

/* Return the number of stored elements. */
size_t ckit_linked_list_size(const ckit_linked_list *list);

/* Return whether the linked list has zero elements. */
bool ckit_linked_list_is_empty(const ckit_linked_list *list);

#endif
