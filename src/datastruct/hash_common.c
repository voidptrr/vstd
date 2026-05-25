#include <string.h>

#include "ckit/common/panic.h"
#include "ckit/datastruct/linked_list.h"
#include "ckit/memory/allocators/allocator.h"
#include "datastruct/hash_common.h"

ckit_linked_list **ckit_hash_buckets_init(size_t capacity, ckit_allocator *allocator) {
    CKIT_ASSERT(capacity > 0, "fatal: ckit_hash_buckets_init invalid arguments");

    size_t alloc_size = sizeof(ckit_linked_list *) * capacity;
    ckit_linked_list **buckets = (ckit_linked_list **)ckit_malloc(allocator, alloc_size);
    memset((void *)buckets, 0, alloc_size);

    for (size_t i = 0; i < capacity; i++) {
        buckets[i] = ckit_linked_list_init(allocator);
    }

    return buckets;
}

void ckit_hash_buckets_free(ckit_linked_list **buckets, size_t capacity, ckit_allocator *allocator,
                            ckit_hash_entry_free_fn entry_free) {
    CKIT_ASSERT(buckets != NULL, "fatal: ckit_hash_buckets_free invalid arguments");
    CKIT_ASSERT(entry_free != NULL, "fatal: ckit_hash_buckets_free invalid arguments");

    for (size_t i = 0; i < capacity; i++) {
        ckit_linked_list_node *node = ckit_linked_list_head(buckets[i]);
        while (node != NULL) {
            ckit_linked_list_node *next = node->next;
            entry_free(node, allocator);
            node = next;
        }
        ckit_linked_list_free(buckets[i]);
    }

    ckit_dealloc(allocator, (void *)buckets);
}

ckit_linked_list **ckit_hash_buckets_rehash(ckit_linked_list **buckets, size_t capacity,
                                            size_t new_capacity, size_t value_size,
                                            ckit_allocator *allocator,
                                            ckit_hash_entry_value_fn entry_value) {
    CKIT_ASSERT(buckets != NULL, "fatal: ckit_hash_buckets_rehash invalid arguments");
    CKIT_ASSERT(capacity > 0, "fatal: ckit_hash_buckets_rehash invalid arguments");
    CKIT_ASSERT(new_capacity > 0, "fatal: ckit_hash_buckets_rehash invalid arguments");
    CKIT_ASSERT(value_size > 0, "fatal: ckit_hash_buckets_rehash invalid arguments");
    CKIT_ASSERT(entry_value != NULL, "fatal: ckit_hash_buckets_rehash invalid arguments");

    ckit_linked_list **new_buckets = ckit_hash_buckets_init(new_capacity, allocator);
    for (size_t i = 0; i < capacity; i++) {
        ckit_linked_list_node *node = ckit_linked_list_head(buckets[i]);
        while (node != NULL) {
            ckit_linked_list_node *next = node->next;
            const void *value = entry_value(node);
            size_t bucket = ckit_hash_bucket_index(value, value_size, new_capacity);

            ckit_linked_list_pushfront(new_buckets[bucket], node);
            node = next;
        }
        ckit_linked_list_free(buckets[i]);
    }

    ckit_dealloc(allocator, (void *)buckets);
    return new_buckets;
}

ckit_linked_list_node *ckit_hash_bucket_find(ckit_linked_list *bucket, const void *value,
                                             size_t value_size, ckit_eq_fn value_eq,
                                             ckit_hash_entry_value_fn entry_value) {
    CKIT_ASSERT(bucket != NULL, "fatal: ckit_hash_bucket_find invalid arguments");
    CKIT_ASSERT(value != NULL, "fatal: ckit_hash_bucket_find invalid arguments");
    CKIT_ASSERT(value_size > 0, "fatal: ckit_hash_bucket_find invalid arguments");
    CKIT_ASSERT(value_eq != NULL, "fatal: ckit_hash_bucket_find invalid arguments");
    CKIT_ASSERT(entry_value != NULL, "fatal: ckit_hash_bucket_find invalid arguments");

    ckit_linked_list_node *node = ckit_linked_list_head(bucket);
    while (node != NULL) {
        if (value_eq(entry_value(node), value, value_size)) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

ckit_linked_list_node *ckit_hash_bucket_remove(ckit_linked_list *bucket, const void *value,
                                               size_t value_size, ckit_eq_fn value_eq,
                                               ckit_hash_entry_value_fn entry_value) {
    CKIT_ASSERT(bucket != NULL, "fatal: ckit_hash_bucket_remove invalid arguments");
    CKIT_ASSERT(value != NULL, "fatal: ckit_hash_bucket_remove invalid arguments");
    CKIT_ASSERT(value_size > 0, "fatal: ckit_hash_bucket_remove invalid arguments");
    CKIT_ASSERT(value_eq != NULL, "fatal: ckit_hash_bucket_remove invalid arguments");
    CKIT_ASSERT(entry_value != NULL, "fatal: ckit_hash_bucket_remove invalid arguments");

    ckit_linked_list_node *prev = NULL;
    ckit_linked_list_node *node = ckit_linked_list_head(bucket);
    while (node != NULL) {
        if (value_eq(entry_value(node), value, value_size)) {
            ckit_linked_list_remove_after(bucket, prev);
            return node;
        }
        prev = node;
        node = node->next;
    }

    return NULL;
}
