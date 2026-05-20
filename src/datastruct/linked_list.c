#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ckit/datastruct/linked_list.h"
#include "ckit/mem/allocators/allocator.h"

static void *ckit_linked_list_alloc(const ckit_linked_list *list, size_t size) {
    if (list->allocator == NULL || list->allocator->alloc == NULL) {
        return ckit_malloc(size);
    }
    return list->allocator->alloc(list->allocator->ctx, size);
}

static void ckit_linked_list_dealloc(const ckit_linked_list *list, void *ptr) {
    if (list->allocator == NULL || list->allocator->dealloc == NULL) {
        free(ptr);
        return;
    }
    list->allocator->dealloc(list->allocator->ctx, ptr);
}

void ckit_linked_list_init(ckit_linked_list *list, size_t elem_size, ckit_allocator *allocator) {
    assert(list != NULL);
    assert(elem_size > 0U);

    list->size = 0;
    list->elem_size = elem_size;
    list->head = NULL;
    list->tail = NULL;
    list->allocator = allocator;
}

void ckit_linked_list_push(ckit_linked_list *list, const void *element) {
    assert(list != NULL);
    assert(element != NULL);

    ckit_linked_list_node *new_node = ckit_linked_list_alloc(list, sizeof(ckit_linked_list_node));
    if (new_node == NULL) {
        ckit_panic("fatal: ckit_linked_list_push allocation failed");
    }

    new_node->data = ckit_linked_list_alloc(list, list->elem_size);
    if (new_node->data == NULL) {
        ckit_linked_list_dealloc(list, new_node);
        ckit_panic("fatal: ckit_linked_list_push allocation failed");
    }

    memcpy(new_node->data, element, list->elem_size);
    new_node->next = NULL;

    if (list->tail != NULL) {
        list->tail->next = new_node;
    } else {
        list->head = new_node;
    }

    list->tail = new_node;
    list->size += 1;
}

void ckit_linked_list_pushfront(ckit_linked_list *list, const void *element) {
    assert(list != NULL);
    assert(element != NULL);

    ckit_linked_list_node *new_node = ckit_linked_list_alloc(list, sizeof(ckit_linked_list_node));
    if (new_node == NULL) {
        ckit_panic("fatal: ckit_linked_list_pushfront allocation failed");
    }

    new_node->data = ckit_linked_list_alloc(list, list->elem_size);
    if (new_node->data == NULL) {
        ckit_linked_list_dealloc(list, new_node);
        ckit_panic("fatal: ckit_linked_list_pushfront allocation failed");
    }

    memcpy(new_node->data, element, list->elem_size);
    new_node->next = list->head;
    list->head = new_node;

    if (list->tail == NULL) {
        list->tail = new_node;
    }

    list->size += 1;
}

void *ckit_linked_list_popleft(ckit_linked_list *list) {
    assert(list != NULL);

    if (list->head == NULL) {
        return NULL;
    }

    ckit_linked_list_node *old_head = list->head;
    void *out = old_head->data;
    list->head = old_head->next;

    if (list->head == NULL) {
        list->tail = NULL;
    }

    ckit_linked_list_dealloc(list, old_head);
    list->size -= 1;
    return out;
}

void ckit_linked_list_free(ckit_linked_list *list) {
    assert(list != NULL);

    ckit_linked_list_node *curr = list->head;
    while (curr != NULL) {
        ckit_linked_list_node *next = curr->next;
        ckit_linked_list_dealloc(list, curr->data);
        ckit_linked_list_dealloc(list, curr);
        curr = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->allocator = NULL;
}

size_t ckit_linked_list_size(const ckit_linked_list *list) {
    if (list == NULL) {
        return 0;
    }

    return list->size;
}

bool ckit_linked_list_is_empty(const ckit_linked_list *list) {
    return ckit_linked_list_size(list) == 0;
}
