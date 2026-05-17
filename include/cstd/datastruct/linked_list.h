#ifndef CSTD_DATASTRUCT_LINKED_LIST_H
#define CSTD_DATASTRUCT_LINKED_LIST_H

#include <stdbool.h>
#include <stddef.h>

#include "cstd/status.h"

/*
 * Generic singly linked list.
 * +----------------------+----------------------+----------------------+
 * | size (size_t)        | elem_size (size_t)   | head (node *)        |
 * +----------------------+----------------------+----------------------+
 * | tail (node *)        |                      |                      |
 * +----------------------+----------------------+----------------------+
 */

typedef struct cstd_linked_list_node {
    struct cstd_linked_list_node *next;
    void *data;
} cstd_linked_list_node;

typedef struct cstd_linked_list {
    size_t size;
    size_t elem_size;
    cstd_linked_list_node *head;
    cstd_linked_list_node *tail;
} cstd_linked_list;

/* Initialize a linked list with element size elem_size. */
cstd_status cstd_linked_list_init(cstd_linked_list *list, size_t elem_size);

/* Append one element by copying elem_size bytes from element. */
cstd_status cstd_linked_list_push(cstd_linked_list *list, const void *element);

/* Prepend one element by copying elem_size bytes from element. */
cstd_status cstd_linked_list_pushfront(cstd_linked_list *list, const void *element);

/* Remove the first element and copy it into out. */
cstd_status cstd_linked_list_popleft(cstd_linked_list *list, void *out);

/* Release owned nodes and reset list to an empty state. */
cstd_status cstd_linked_list_free(cstd_linked_list *list);

/* Return the number of stored elements. */
size_t cstd_linked_list_size(const cstd_linked_list *list);

/* Return whether the linked list has zero elements. */
bool cstd_linked_list_is_empty(const cstd_linked_list *list);

#endif
