#include <string.h>

#include "ckit/common/panic.h"
#include "ckit/datastruct/linked_list.h"
#include "ckit/memory/allocators/allocator.h"

typedef struct ckit_linked_list_node {
    struct ckit_linked_list_node *next;
    void *data;
} ckit_linked_list_node;

struct ckit_linked_list {
    size_t size;
    size_t elem_size;
    ckit_linked_list_node *head;
    ckit_linked_list_node *tail;
    ckit_allocator *allocator;
};

ckit_linked_list *ckit_linked_list_init(size_t elem_size, ckit_allocator *allocator) {
    CKIT_ASSERT(elem_size > 0U, "fatal: ckit_linked_list_init invalid arguments");

    ckit_linked_list *list = ckit_malloc(allocator, sizeof(*list));
    list->size = 0;
    list->elem_size = elem_size;
    list->head = NULL;
    list->tail = NULL;
    list->allocator = allocator;

    return list;
}

void ckit_linked_list_push(ckit_linked_list *list, const void *element) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_linked_list_push invalid arguments");
    CKIT_ASSERT(element != NULL, "fatal: ckit_linked_list_push invalid arguments");

    ckit_linked_list_node *new_node = ckit_malloc(list->allocator, sizeof(*new_node));
    new_node->data = ckit_malloc(list->allocator, list->elem_size);

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
    CKIT_ASSERT(list != NULL, "fatal: ckit_linked_list_pushfront invalid arguments");
    CKIT_ASSERT(element != NULL, "fatal: ckit_linked_list_pushfront invalid arguments");

    ckit_linked_list_node *new_node = ckit_malloc(list->allocator, sizeof(*new_node));
    new_node->data = ckit_malloc(list->allocator, list->elem_size);

    memcpy(new_node->data, element, list->elem_size);
    new_node->next = list->head;
    list->head = new_node;

    if (list->tail == NULL) {
        list->tail = new_node;
    }

    list->size += 1;
}

void *ckit_linked_list_popleft(ckit_linked_list *list) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_linked_list_popleft invalid arguments");

    if (list->head == NULL) {
        return NULL;
    }

    ckit_linked_list_node *old_head = list->head;
    void *out = old_head->data;
    list->head = old_head->next;

    if (list->head == NULL) {
        list->tail = NULL;
    }

    ckit_dealloc(list->allocator, old_head);
    list->size -= 1;
    return out;
}

void ckit_linked_list_free(ckit_linked_list *list) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_linked_list_free invalid arguments");

    ckit_allocator *allocator = list->allocator;
    ckit_linked_list_node *curr = list->head;
    while (curr != NULL) {
        ckit_linked_list_node *next = curr->next;
        ckit_dealloc(list->allocator, curr->data);
        ckit_dealloc(list->allocator, curr);
        curr = next;
    }

    ckit_dealloc(allocator, list);
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
