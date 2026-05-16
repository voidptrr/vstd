#include <stdlib.h>
#include <string.h>

#include "cstd/datastruct/linked_list.h"
#include "cstd/mem/allocator.h"

cstd_status cstd_linked_list_init(cstd_linked_list *list, size_t elem_size) {
    if (list == NULL) {
        return CSTD_ERR_NULL;
    }

    if (elem_size == 0) {
        return CSTD_ERR_RANGE;
    }

    list->size = 0;
    list->elem_size = elem_size;
    list->head = NULL;
    list->tail = NULL;

    return CSTD_OK;
}

cstd_status cstd_linked_list_push(cstd_linked_list *list, const void *element) {
    if (list == NULL || element == NULL) {
        return CSTD_ERR_NULL;
    }

    cstd_linked_list_node *new_node = cstd_malloc(sizeof(cstd_linked_list_node));

    new_node->data = cstd_malloc(list->elem_size);

    memcpy(new_node->data, element, list->elem_size);
    new_node->next = NULL;

    if (list->tail != NULL) {
        list->tail->next = new_node;
    } else {
        list->head = new_node;
    }

    list->tail = new_node;
    list->size += 1;

    return CSTD_OK;
}

cstd_status cstd_linked_list_pushfront(cstd_linked_list *list, const void *element) {
    if (list == NULL || element == NULL) {
        return CSTD_ERR_NULL;
    }

    cstd_linked_list_node *new_node = cstd_malloc(sizeof(cstd_linked_list_node));

    new_node->data = cstd_malloc(list->elem_size);

    memcpy(new_node->data, element, list->elem_size);
    new_node->next = list->head;
    list->head = new_node;

    if (list->tail == NULL) {
        list->tail = new_node;
    }

    list->size += 1;
    return CSTD_OK;
}

cstd_status cstd_linked_list_popleft(cstd_linked_list *list, void *out) {
    if (list == NULL || out == NULL) {
        return CSTD_ERR_NULL;
    }

    if (list->head == NULL) {
        return CSTD_ERR_EMPTY;
    }

    cstd_linked_list_node *old_head = list->head;
    memcpy(out, old_head->data, list->elem_size);
    list->head = old_head->next;

    if (list->head == NULL) {
        list->tail = NULL;
    }

    free(old_head->data);
    free(old_head);
    list->size -= 1;

    return CSTD_OK;
}

cstd_status cstd_linked_list_free(cstd_linked_list *list) {
    if (list == NULL) {
        return CSTD_ERR_NULL;
    }

    cstd_linked_list_node *curr = list->head;
    while (curr != NULL) {
        cstd_linked_list_node *next = curr->next;
        free(curr->data);
        free(curr);
        curr = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;

    return CSTD_OK;
}

size_t cstd_linked_list_size(const cstd_linked_list *list) {
    if (list == NULL) {
        return 0;
    }

    return list->size;
}

bool cstd_linked_list_is_empty(const cstd_linked_list *list) {
    return cstd_linked_list_size(list) == 0;
}
