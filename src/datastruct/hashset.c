#include <string.h>

#include "ckit/common/panic.h"
#include "ckit/datastruct/hashset.h"
#include "ckit/datastruct/linked_list.h"
#include "ckit/memory/allocators/allocator.h"
#include "ckit/memory/utils.h"
#include "datastruct/hash_common.h"

typedef struct ckit_hashset_entry {
    ckit_linked_list_node node;
    void *elem;
} ckit_hashset_entry;

struct ckit_hashset {
    size_t size;
    size_t elem_size;
    size_t capacity;
    ckit_linked_list **buckets;
    ckit_hashset_elem_eq_fn elem_eq;
    ckit_allocator *allocator;
};

static void ckit_hashset_entry_free(ckit_linked_list_node *node, ckit_allocator *allocator) {
    ckit_hashset_entry *entry = CKIT_CONTAINER_OF(node, ckit_hashset_entry, node);
    ckit_dealloc(allocator, entry->elem);
    ckit_dealloc(allocator, entry);
}

static const void *ckit_hashset_entry_elem(const ckit_linked_list_node *node) {
    const ckit_hashset_entry *entry = CKIT_CONTAINER_OF(node, ckit_hashset_entry, node);
    return entry->elem;
}

static ckit_hashset_entry *ckit_hashset_entry_get(const ckit_hashset *set, const void *elem) {
    size_t bucket = ckit_hash_bucket_index(elem, set->elem_size, set->capacity);
    ckit_linked_list_node *node = ckit_hash_bucket_find(set->buckets[bucket], elem, set->elem_size,
                                                        set->elem_eq, ckit_hashset_entry_elem);
    if (node != NULL) {
        return CKIT_CONTAINER_OF(node, ckit_hashset_entry, node);
    }
    return NULL;
}

ckit_hashset *ckit_hashset_init(size_t elem_size, ckit_hashset_elem_eq_fn elem_eq,
                                ckit_allocator *allocator) {
    CKIT_ASSERT(elem_eq != NULL, "fatal: ckit_hashset_init invalid arguments");
    CKIT_ASSERT(elem_size > 0, "fatal: ckit_hashset_init invalid arguments");

    ckit_hashset *set = ckit_malloc(allocator, sizeof(*set));
    set->allocator = allocator;
    set->buckets = ckit_hash_buckets_init(CKIT_HASH_DEFAULT_CAPACITY, allocator);

    set->size = 0;
    set->elem_size = elem_size;
    set->capacity = CKIT_HASH_DEFAULT_CAPACITY;
    set->elem_eq = elem_eq;

    return set;
}

void ckit_hashset_insert(ckit_hashset *set, const void *elem) {
    CKIT_ASSERT(set != NULL, "fatal: ckit_hashset_insert invalid arguments");
    CKIT_ASSERT(elem != NULL, "fatal: ckit_hashset_insert invalid arguments");

    size_t bucket = ckit_hash_bucket_index(elem, set->elem_size, set->capacity);
    ckit_linked_list_node *node = ckit_hash_bucket_find(set->buckets[bucket], elem, set->elem_size,
                                                        set->elem_eq, ckit_hashset_entry_elem);
    if (node != NULL) {
        return;
    }

    if (ckit_hash_should_grow(set->size, set->capacity)) {
        size_t new_capacity = set->capacity * 2;
        set->buckets =
            ckit_hash_buckets_rehash(set->buckets, set->capacity, new_capacity, set->elem_size,
                                     set->allocator, ckit_hashset_entry_elem);
        set->capacity = new_capacity;
        bucket = ckit_hash_bucket_index(elem, set->elem_size, set->capacity);
    }

    ckit_hashset_entry *entry = ckit_malloc(set->allocator, sizeof(*entry));
    entry->elem = ckit_malloc(set->allocator, set->elem_size);
    memcpy(entry->elem, elem, set->elem_size);

    ckit_linked_list_pushfront(set->buckets[bucket], &entry->node);
    set->size += 1;
}

bool ckit_hashset_contains(const ckit_hashset *set, const void *elem) {
    CKIT_ASSERT(set != NULL, "fatal: ckit_hashset_contains invalid arguments");
    CKIT_ASSERT(elem != NULL, "fatal: ckit_hashset_contains invalid arguments");

    return ckit_hashset_entry_get(set, elem) != NULL;
}

void *ckit_hashset_get(ckit_hashset *set, const void *elem) {
    CKIT_ASSERT(set != NULL, "fatal: ckit_hashset_get invalid arguments");
    CKIT_ASSERT(elem != NULL, "fatal: ckit_hashset_get invalid arguments");

    ckit_hashset_entry *entry = ckit_hashset_entry_get(set, elem);
    if (entry != NULL) {
        return entry->elem;
    }
    return NULL;
}

const void *ckit_hashset_get_const(const ckit_hashset *set, const void *elem) {
    CKIT_ASSERT(set != NULL, "fatal: ckit_hashset_get_const invalid arguments");
    CKIT_ASSERT(elem != NULL, "fatal: ckit_hashset_get_const invalid arguments");

    ckit_hashset_entry *entry = ckit_hashset_entry_get(set, elem);
    if (entry != NULL) {
        return entry->elem;
    }
    return NULL;
}

void ckit_hashset_remove(ckit_hashset *set, const void *elem) {
    CKIT_ASSERT(set != NULL, "fatal: ckit_hashset_remove invalid arguments");
    CKIT_ASSERT(elem != NULL, "fatal: ckit_hashset_remove invalid arguments");

    size_t bucket = ckit_hash_bucket_index(elem, set->elem_size, set->capacity);
    ckit_linked_list_node *node = ckit_hash_bucket_remove(
        set->buckets[bucket], elem, set->elem_size, set->elem_eq, ckit_hashset_entry_elem);
    if (node != NULL) {
        ckit_hashset_entry *entry = CKIT_CONTAINER_OF(node, ckit_hashset_entry, node);
        ckit_dealloc(set->allocator, entry->elem);
        ckit_dealloc(set->allocator, entry);
        set->size -= 1;
    }
}

void ckit_hashset_free(ckit_hashset *set) {
    CKIT_ASSERT(set != NULL, "fatal: ckit_hashset_free invalid arguments");

    ckit_allocator *allocator = set->allocator;
    ckit_hash_buckets_free(set->buckets, set->capacity, allocator, ckit_hashset_entry_free);
    ckit_dealloc(allocator, set);
}

size_t ckit_hashset_size(const ckit_hashset *set) {
    CKIT_ASSERT(set != NULL, "fatal: ckit_hashset_size invalid arguments");

    return set->size;
}
