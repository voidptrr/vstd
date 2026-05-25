#include "ckit/common/panic.h"
#include "ckit/datastruct/doubly_linked_list.h"
#include "ckit/memory/allocators/allocator.h"

struct ckit_doubly_linked_list {
    size_t size;
    ckit_doubly_linked_list_node *head;
    ckit_doubly_linked_list_node *tail;
    ckit_allocator *allocator;
};

ckit_doubly_linked_list *ckit_doubly_linked_list_init(ckit_allocator *allocator) {
    ckit_doubly_linked_list *list = ckit_malloc(allocator, sizeof(*list));
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->allocator = allocator;

    return list;
}

void ckit_doubly_linked_list_push(ckit_doubly_linked_list *list,
                                  ckit_doubly_linked_list_node *node) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_doubly_linked_list_push invalid arguments");
    CKIT_ASSERT(node != NULL, "fatal: ckit_doubly_linked_list_push invalid arguments");

    node->prev = list->tail;
    node->next = NULL;

    if (list->tail != NULL) {
        list->tail->next = node;
    } else {
        list->head = node;
    }

    list->tail = node;
    list->size += 1;
}

void ckit_doubly_linked_list_pushfront(ckit_doubly_linked_list *list,
                                       ckit_doubly_linked_list_node *node) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_doubly_linked_list_pushfront invalid arguments");
    CKIT_ASSERT(node != NULL, "fatal: ckit_doubly_linked_list_pushfront invalid arguments");

    node->prev = NULL;
    node->next = list->head;

    if (list->head != NULL) {
        list->head->prev = node;
    } else {
        list->tail = node;
    }

    list->head = node;
    list->size += 1;
}

void ckit_doubly_linked_list_insert_after(ckit_doubly_linked_list *list,
                                          ckit_doubly_linked_list_node *after,
                                          ckit_doubly_linked_list_node *node) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_doubly_linked_list_insert_after invalid arguments");
    CKIT_ASSERT(node != NULL, "fatal: ckit_doubly_linked_list_insert_after invalid arguments");

    if (after == NULL) {
        ckit_doubly_linked_list_pushfront(list, node);
        return;
    }

    node->prev = after;
    node->next = after->next;

    if (after->next != NULL) {
        after->next->prev = node;
    } else {
        list->tail = node;
    }

    after->next = node;
    list->size += 1;
}

ckit_doubly_linked_list_node *ckit_doubly_linked_list_popleft(ckit_doubly_linked_list *list) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_doubly_linked_list_popleft invalid arguments");

    if (list->head == NULL) {
        return NULL;
    }

    ckit_doubly_linked_list_node *old_head = list->head;
    list->head = old_head->next;

    if (list->head != NULL) {
        list->head->prev = NULL;
    } else {
        list->tail = NULL;
    }

    old_head->prev = NULL;
    old_head->next = NULL;
    list->size -= 1;
    return old_head;
}

ckit_doubly_linked_list_node *ckit_doubly_linked_list_popback(ckit_doubly_linked_list *list) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_doubly_linked_list_popback invalid arguments");

    if (list->tail == NULL) {
        return NULL;
    }

    ckit_doubly_linked_list_node *old_tail = list->tail;
    list->tail = old_tail->prev;

    if (list->tail != NULL) {
        list->tail->next = NULL;
    } else {
        list->head = NULL;
    }

    old_tail->prev = NULL;
    old_tail->next = NULL;
    list->size -= 1;
    return old_tail;
}

void ckit_doubly_linked_list_remove(ckit_doubly_linked_list *list,
                                    ckit_doubly_linked_list_node *node) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_doubly_linked_list_remove invalid arguments");
    CKIT_ASSERT(node != NULL, "fatal: ckit_doubly_linked_list_remove invalid arguments");

    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }

    if (node->next != NULL) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }

    node->prev = NULL;
    node->next = NULL;
    list->size -= 1;
}

size_t ckit_doubly_linked_list_size(const ckit_doubly_linked_list *list) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_doubly_linked_list_size invalid arguments");

    return list->size;
}

ckit_doubly_linked_list_node *ckit_doubly_linked_list_head(const ckit_doubly_linked_list *list) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_doubly_linked_list_head invalid arguments");

    return list->head;
}

ckit_doubly_linked_list_node *ckit_doubly_linked_list_tail(const ckit_doubly_linked_list *list) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_doubly_linked_list_tail invalid arguments");

    return list->tail;
}

void ckit_doubly_linked_list_deinit(ckit_doubly_linked_list *list) {
    CKIT_ASSERT(list != NULL, "fatal: ckit_doubly_linked_list_deinit invalid arguments");

    ckit_allocator *allocator = list->allocator;
    ckit_dealloc(allocator, list);
}
