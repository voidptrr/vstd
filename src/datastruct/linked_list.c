#include "ckit/common/panic.h"
#include "ckit/datastruct/linked_list.h"
#include "ckit/memory/allocators/allocator.h"

struct ck_linked_list {
    size_t size;
    ck_linked_list_node *head;
    ck_linked_list_node *tail;
    ck_allocator *allocator;
};

ck_linked_list *ck_linked_list_init(ck_allocator *allocator) {
    ck_linked_list *list = ck_malloc(allocator, sizeof(ck_linked_list));
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->allocator = allocator;

    return list;
}

void ck_linked_list_push(ck_linked_list *list, ck_linked_list_node *node) {
    CK_ASSERT(list != NULL, "fatal: ck_linked_list_push invalid arguments");
    CK_ASSERT(node != NULL, "fatal: ck_linked_list_push invalid arguments");

    node->next = NULL;
    if (list->tail != NULL) {
        list->tail->next = node;
    } else {
        list->head = node;
    }

    list->tail = node;
    list->size += 1;
}

void ck_linked_list_pushfront(ck_linked_list *list, ck_linked_list_node *node) {
    CK_ASSERT(list != NULL, "fatal: ck_linked_list_pushfront invalid arguments");
    CK_ASSERT(node != NULL, "fatal: ck_linked_list_pushfront invalid arguments");

    node->next = list->head;
    list->head = node;

    if (list->tail == NULL) {
        list->tail = node;
    }

    list->size += 1;
}

ck_linked_list_node *ck_linked_list_popleft(ck_linked_list *list) {
    CK_ASSERT(list != NULL, "fatal: ck_linked_list_popleft invalid arguments");

    if (list->head == NULL) {
        return NULL;
    }

    ck_linked_list_node *old_head = list->head;
    list->head = old_head->next;
    old_head->next = NULL;

    if (list->head == NULL) {
        list->tail = NULL;
    }

    list->size -= 1;
    return old_head;
}

ck_linked_list_node *ck_linked_list_remove_after(ck_linked_list *list, ck_linked_list_node *prev) {
    CK_ASSERT(list != NULL, "fatal: ck_linked_list_remove_after invalid arguments");

    if (prev == NULL) {
        return ck_linked_list_popleft(list);
    }

    ck_linked_list_node *node = prev->next;
    if (node == NULL) {
        return NULL;
    }

    prev->next = node->next;
    if (list->tail == node) {
        list->tail = prev;
    }

    node->next = NULL;
    list->size -= 1;
    return node;
}

size_t ck_linked_list_size(const ck_linked_list *list) {
    CK_ASSERT(list != NULL, "fatal: ck_linked_list_size invalid arguments");

    return list->size;
}

ck_linked_list_node *ck_linked_list_head(const ck_linked_list *list) {
    CK_ASSERT(list != NULL, "fatal: ck_linked_list_head invalid arguments");

    return list->head;
}

void ck_linked_list_deinit(ck_linked_list *list) {
    CK_ASSERT(list != NULL, "fatal: ck_linked_list_deinit invalid arguments");

    ck_allocator *allocator = list->allocator;
    ck_dealloc(allocator, list);
}
