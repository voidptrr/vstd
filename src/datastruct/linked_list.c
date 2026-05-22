#include "ckit/common/panic.h"
#include "ckit/datastruct/linked_list.h"
#include "ckit/memory/allocators/allocator.h"

struct ckit_linked_list {
    size_t size;
    ckit_linked_list_node *head;
    ckit_linked_list_node *tail;
    ckit_allocator *allocator;
};

ckit_linked_list *ckit_linked_list_init(ckit_allocator *allocator) {
    ckit_linked_list *list = ckit_malloc(allocator, sizeof(*list));
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->allocator = allocator;

    return list;
}

void ckit_linked_list_node_init(ckit_linked_list_node *node) {
    CKIT_ASSERT(node != NULL, "fatal: ckit_linked_list_node_init invalid arguments");

    node->next = NULL;
}

void ckit_linked_list_push(ckit_linked_list *list, ckit_linked_list_node *node) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_linked_list_push invalid arguments");
    CKIT_ASSERT(node != NULL, "fatal: ckit_linked_list_push invalid arguments");

    node->next = NULL;
    if (list->tail != NULL) {
        list->tail->next = node;
    } else {
        list->head = node;
    }

    list->tail = node;
    list->size += 1;
}

void ckit_linked_list_pushfront(ckit_linked_list *list, ckit_linked_list_node *node) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_linked_list_pushfront invalid arguments");
    CKIT_ASSERT(node != NULL, "fatal: ckit_linked_list_pushfront invalid arguments");

    node->next = list->head;
    list->head = node;

    if (list->tail == NULL) {
        list->tail = node;
    }

    list->size += 1;
}

ckit_linked_list_node *ckit_linked_list_popleft(ckit_linked_list *list) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_linked_list_popleft invalid arguments");

    if (list->head == NULL) {
        return NULL;
    }

    ckit_linked_list_node *old_head = list->head;
    list->head = old_head->next;
    old_head->next = NULL;

    if (list->head == NULL) {
        list->tail = NULL;
    }

    list->size -= 1;
    return old_head;
}

void ckit_linked_list_free(ckit_linked_list *list) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_linked_list_free invalid arguments");

    ckit_allocator *allocator = list->allocator;
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
