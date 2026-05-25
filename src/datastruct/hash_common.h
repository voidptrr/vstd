#ifndef CK_DATASTRUCT_HASH_COMMON_H
#define CK_DATASTRUCT_HASH_COMMON_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/compare.h"
#include "ckit/datastruct/linked_list.h"
#include "ckit/memory/allocators/allocator.h"
#include "crypto/fnv1a.h"

#define CK_HASH_COMMON_DEFAULT_CAPACITY 16
#define CK_HASH_COMMON_MAX_LOAD 0.75

typedef void (*ck_hash_common_entry_deinit_fn)(ck_linked_list_node *node, ck_allocator *allocator);
typedef const void *(*ck_hash_common_entry_value_fn)(const ck_linked_list_node *node);

ck_linked_list **ck_hash_common_buckets_init(size_t capacity, ck_allocator *allocator);
ck_linked_list **ck_hash_common_buckets_rehash(ck_linked_list **buckets, size_t capacity,
                                               size_t new_capacity, size_t value_size,
                                               ck_allocator *allocator,
                                               ck_hash_common_entry_value_fn entry_value);
ck_linked_list_node *ck_hash_common_bucket_find(ck_linked_list *bucket, const void *value,
                                                size_t value_size, ck_eq_fn value_eq,
                                                ck_hash_common_entry_value_fn entry_value);
ck_linked_list_node *ck_hash_common_bucket_remove(ck_linked_list *bucket, const void *value,
                                                  size_t value_size, ck_eq_fn value_eq,
                                                  ck_hash_common_entry_value_fn entry_value);
void ck_hash_common_buckets_deinit(ck_linked_list **buckets, size_t capacity,
                                   ck_allocator *allocator,
                                   ck_hash_common_entry_deinit_fn entry_deinit);

static inline size_t ck_hash_common_bucket_index(const void *value, size_t value_size,
                                                 size_t capacity) {
    return ck_internal_fnv1a_hash(value, value_size) % capacity;
}

static inline bool ck_hash_common_should_grow(size_t size, size_t capacity) {
    return ((double)(size + 1) / (double)capacity) > CK_HASH_COMMON_MAX_LOAD;
}

#endif
