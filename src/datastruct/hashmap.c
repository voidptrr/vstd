#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ckit/common/panic.h"
#include "ckit/datastruct/hashmap.h"
#include "ckit/datastruct/linked_list.h"
#include "ckit/memory/allocators/allocator.h"
#include "datastruct/hash_common.h"
#include "memory/utils.h"

typedef struct ck_hashmap_entry {
    ck_linked_list_node node;
    max_align_t align;
    uint8_t data[];
} ck_hashmap_entry;

struct ck_hashmap {
    size_t size;
    size_t key_size;
    size_t value_size;
    size_t capacity;
    ck_linked_list **buckets;
    ck_hashmap_key_eq_fn key_eq;
    ck_allocator *allocator;
};

static void ck_hashmap_entry_deinit(ck_linked_list_node *node, ck_allocator *allocator) {
    ck_hashmap_entry *entry = CK_CONTAINER_OF(node, ck_hashmap_entry, node);
    ck_dealloc(allocator, entry);
}

static const void *ck_hashmap_entry_key(const ck_linked_list_node *node) {
    const ck_hashmap_entry *entry = CK_CONTAINER_OF(node, ck_hashmap_entry, node);
    return entry->data;
}

static void *ck_hashmap_entry_value(const ck_hashmap *map, ck_hashmap_entry *entry) {
    size_t value_offset = ck_align_up(map->key_size, CK_MEMORY_ALIGN);
    return entry->data + value_offset;
}

static const void *ck_hashmap_entry_value_const(const ck_hashmap *map,
                                                const ck_hashmap_entry *entry) {
    size_t value_offset = ck_align_up(map->key_size, CK_MEMORY_ALIGN);
    return entry->data + value_offset;
}

static ck_hashmap_entry *ck_hashmap_entry_get(const ck_hashmap *map, const void *key) {
    size_t bucket = ck_hash_common_bucket_index(key, map->key_size, map->capacity);
    ck_linked_list_node *node = ck_hash_common_bucket_find(map->buckets[bucket], key, map->key_size,
                                                           map->key_eq, ck_hashmap_entry_key);
    if (node != NULL) {
        return CK_CONTAINER_OF(node, ck_hashmap_entry, node);
    }
    return NULL;
}

ck_hashmap *ck_hashmap_init(size_t key_size, size_t value_size, ck_hashmap_key_eq_fn key_eq,
                            ck_allocator *allocator) {
    CK_ASSERT(key_eq != NULL, "fatal: ck_hashmap_init invalid arguments");
    CK_ASSERT(key_size > 0, "fatal: ck_hashmap_init invalid arguments");
    CK_ASSERT(value_size > 0, "fatal: ck_hashmap_init invalid arguments");

    ck_hashmap *map = ck_malloc(allocator, sizeof(ck_hashmap));
    map->allocator = allocator;
    map->buckets = ck_hash_common_buckets_init(CK_HASH_COMMON_DEFAULT_CAPACITY, allocator);

    map->size = 0;
    map->key_size = key_size;
    map->value_size = value_size;
    map->capacity = CK_HASH_COMMON_DEFAULT_CAPACITY;
    map->key_eq = key_eq;

    return map;
}

void ck_hashmap_put(ck_hashmap *map, const void *key, const void *value) {
    CK_ASSERT(map != NULL, "fatal: ck_hashmap_put invalid arguments");
    CK_ASSERT(key != NULL, "fatal: ck_hashmap_put invalid arguments");
    CK_ASSERT(value != NULL, "fatal: ck_hashmap_put invalid arguments");

    size_t bucket = ck_hash_common_bucket_index(key, map->key_size, map->capacity);
    ck_linked_list_node *node = ck_hash_common_bucket_find(map->buckets[bucket], key, map->key_size,
                                                           map->key_eq, ck_hashmap_entry_key);
    if (node != NULL) {
        ck_hashmap_entry *entry = CK_CONTAINER_OF(node, ck_hashmap_entry, node);
        memcpy(ck_hashmap_entry_value(map, entry), value, map->value_size);
        return;
    }

    if (ck_hash_common_should_grow(map->size, map->capacity)) {
        size_t new_capacity = map->capacity * 2;
        map->buckets =
            ck_hash_common_buckets_rehash(map->buckets, map->capacity, new_capacity, map->key_size,
                                          map->allocator, ck_hashmap_entry_key);
        map->capacity = new_capacity;
        bucket = ck_hash_common_bucket_index(key, map->key_size, map->capacity);
    }

    size_t value_offset = ck_align_up(map->key_size, CK_MEMORY_ALIGN);
    size_t alloc_size = sizeof(ck_hashmap_entry) + value_offset + map->value_size;
    ck_hashmap_entry *entry = ck_malloc(map->allocator, alloc_size);

    memcpy(entry->data, key, map->key_size);
    memcpy(ck_hashmap_entry_value(map, entry), value, map->value_size);

    ck_linked_list_pushfront(map->buckets[bucket], &entry->node);
    map->size += 1;
}

void *ck_hashmap_get(ck_hashmap *map, const void *key) {
    CK_ASSERT(map != NULL, "fatal: ck_hashmap_get invalid arguments");
    CK_ASSERT(key != NULL, "fatal: ck_hashmap_get invalid arguments");

    ck_hashmap_entry *entry = ck_hashmap_entry_get(map, key);
    if (entry != NULL) {
        return ck_hashmap_entry_value(map, entry);
    }
    return NULL;
}

const void *ck_hashmap_get_const(const ck_hashmap *map, const void *key) {
    CK_ASSERT(map != NULL, "fatal: ck_hashmap_get_const invalid arguments");
    CK_ASSERT(key != NULL, "fatal: ck_hashmap_get_const invalid arguments");

    ck_hashmap_entry *entry = ck_hashmap_entry_get(map, key);
    if (entry != NULL) {
        return ck_hashmap_entry_value_const(map, entry);
    }
    return NULL;
}

void ck_hashmap_remove(ck_hashmap *map, const void *key) {
    CK_ASSERT(map != NULL, "fatal: ck_hashmap_remove invalid arguments");
    CK_ASSERT(key != NULL, "fatal: ck_hashmap_remove invalid arguments");

    size_t bucket = ck_hash_common_bucket_index(key, map->key_size, map->capacity);
    ck_linked_list_node *node = ck_hash_common_bucket_remove(
        map->buckets[bucket], key, map->key_size, map->key_eq, ck_hashmap_entry_key);
    if (node != NULL) {
        ck_hashmap_entry *entry = CK_CONTAINER_OF(node, ck_hashmap_entry, node);
        ck_dealloc(map->allocator, entry);
        map->size -= 1;
    }
}

size_t ck_hashmap_size(const ck_hashmap *map) {
    CK_ASSERT(map != NULL, "fatal: ck_hashmap_size invalid arguments");

    return map->size;
}

void ck_hashmap_deinit(ck_hashmap *map) {
    CK_ASSERT(map != NULL, "fatal: ck_hashmap_deinit invalid arguments");

    ck_allocator *allocator = map->allocator;
    ck_hash_common_buckets_deinit(map->buckets, map->capacity, allocator, ck_hashmap_entry_deinit);
    ck_dealloc(allocator, map);
}
