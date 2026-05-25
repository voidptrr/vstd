#ifndef CKIT_DATASTRUCT_HASH_COMMON_H
#define CKIT_DATASTRUCT_HASH_COMMON_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/compare.h"
#include "ckit/datastruct/linked_list.h"
#include "ckit/memory/allocators/allocator.h"
#include "crypto/fnv1a.h"

#define CKIT_HASH_DEFAULT_CAPACITY 16
#define CKIT_HASH_MAX_LOAD 0.75

typedef void (*ckit_hash_entry_free_fn)(ckit_linked_list_node *node, ckit_allocator *allocator);
typedef const void *(*ckit_hash_entry_value_fn)(const ckit_linked_list_node *node);

ckit_linked_list **ckit_hash_buckets_init(size_t capacity, ckit_allocator *allocator);
void ckit_hash_buckets_free(ckit_linked_list **buckets, size_t capacity, ckit_allocator *allocator,
                            ckit_hash_entry_free_fn entry_free);
ckit_linked_list **ckit_hash_buckets_rehash(ckit_linked_list **buckets, size_t capacity,
                                            size_t new_capacity, size_t value_size,
                                            ckit_allocator *allocator,
                                            ckit_hash_entry_value_fn entry_value);
ckit_linked_list_node *ckit_hash_bucket_find(ckit_linked_list *bucket, const void *value,
                                             size_t value_size, ckit_eq_fn value_eq,
                                             ckit_hash_entry_value_fn entry_value);
ckit_linked_list_node *ckit_hash_bucket_remove(ckit_linked_list *bucket, const void *value,
                                               size_t value_size, ckit_eq_fn value_eq,
                                               ckit_hash_entry_value_fn entry_value);

static inline size_t ckit_hash_bucket_index(const void *value, size_t value_size, size_t capacity) {
    return ckit_internal_fnv1a_hash(value, value_size) % capacity;
}

static inline bool ckit_hash_should_grow(size_t size, size_t capacity) {
    return ((double)(size + 1) / (double)capacity) > CKIT_HASH_MAX_LOAD;
}

#endif
