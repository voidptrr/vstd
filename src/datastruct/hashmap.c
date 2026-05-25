#include <string.h>

#include "ckit/common/panic.h"
#include "ckit/datastruct/hashmap.h"
#include "ckit/datastruct/linked_list.h"
#include "ckit/memory/allocators/allocator.h"
#include "ckit/memory/utils.h"
#include "datastruct/hash_common.h"

typedef struct ckit_hashmap_entry {
    ckit_linked_list_node node;
    void *key;
    void *value;
} ckit_hashmap_entry;

struct ckit_hashmap {
    size_t size;
    size_t key_size;
    size_t value_size;
    size_t capacity;
    ckit_linked_list **buckets;
    ckit_hashmap_key_eq_fn key_eq;
    ckit_allocator *allocator;
};

static void ckit_hashmap_entry_free(ckit_linked_list_node *node, ckit_allocator *allocator) {
    ckit_hashmap_entry *entry = CKIT_CONTAINER_OF(node, ckit_hashmap_entry, node);
    ckit_dealloc(allocator, entry->key);
    ckit_dealloc(allocator, entry->value);
    ckit_dealloc(allocator, entry);
}

static const void *ckit_hashmap_entry_key(const ckit_linked_list_node *node) {
    const ckit_hashmap_entry *entry = CKIT_CONTAINER_OF(node, ckit_hashmap_entry, node);
    return entry->key;
}

static ckit_hashmap_entry *ckit_hashmap_entry_get(const ckit_hashmap *map, const void *key) {
    size_t bucket = ckit_hash_bucket_index(key, map->key_size, map->capacity);
    ckit_linked_list_node *node = ckit_hash_bucket_find(map->buckets[bucket], key, map->key_size,
                                                        map->key_eq, ckit_hashmap_entry_key);
    if (node != NULL) {
        return CKIT_CONTAINER_OF(node, ckit_hashmap_entry, node);
    }
    return NULL;
}

ckit_hashmap *ckit_hashmap_init(size_t key_size, size_t value_size, ckit_hashmap_key_eq_fn key_eq,
                                ckit_allocator *allocator) {
    CKIT_ASSERT(key_eq != NULL, "fatal: ckit_hashmap_init invalid arguments");
    CKIT_ASSERT(key_size > 0, "fatal: ckit_hashmap_init invalid arguments");
    CKIT_ASSERT(value_size > 0, "fatal: ckit_hashmap_init invalid arguments");

    ckit_hashmap *map = ckit_malloc(allocator, sizeof(*map));
    map->allocator = allocator;
    map->buckets = ckit_hash_buckets_init(CKIT_HASH_DEFAULT_CAPACITY, allocator);

    map->size = 0;
    map->key_size = key_size;
    map->value_size = value_size;
    map->capacity = CKIT_HASH_DEFAULT_CAPACITY;
    map->key_eq = key_eq;

    return map;
}

void ckit_hashmap_put(ckit_hashmap *map, const void *key, const void *value) {
    CKIT_ASSERT(map != NULL, "fatal: ckit_hashmap_put invalid arguments");
    CKIT_ASSERT(key != NULL, "fatal: ckit_hashmap_put invalid arguments");
    CKIT_ASSERT(value != NULL, "fatal: ckit_hashmap_put invalid arguments");

    size_t bucket = ckit_hash_bucket_index(key, map->key_size, map->capacity);
    ckit_linked_list_node *node = ckit_hash_bucket_find(map->buckets[bucket], key, map->key_size,
                                                        map->key_eq, ckit_hashmap_entry_key);
    if (node != NULL) {
        ckit_hashmap_entry *entry = CKIT_CONTAINER_OF(node, ckit_hashmap_entry, node);
        memcpy(entry->value, value, map->value_size);
        return;
    }

    if (ckit_hash_should_grow(map->size, map->capacity)) {
        size_t new_capacity = map->capacity * 2;
        map->buckets =
            ckit_hash_buckets_rehash(map->buckets, map->capacity, new_capacity, map->key_size,
                                     map->allocator, ckit_hashmap_entry_key);
        map->capacity = new_capacity;
        bucket = ckit_hash_bucket_index(key, map->key_size, map->capacity);
    }

    ckit_hashmap_entry *entry = ckit_malloc(map->allocator, sizeof(*entry));
    entry->key = ckit_malloc(map->allocator, map->key_size);
    entry->value = ckit_malloc(map->allocator, map->value_size);

    memcpy(entry->key, key, map->key_size);
    memcpy(entry->value, value, map->value_size);

    ckit_linked_list_pushfront(map->buckets[bucket], &entry->node);
    map->size += 1;
}

void *ckit_hashmap_get(ckit_hashmap *map, const void *key) {
    CKIT_ASSERT(map != NULL, "fatal: ckit_hashmap_get invalid arguments");
    CKIT_ASSERT(key != NULL, "fatal: ckit_hashmap_get invalid arguments");

    ckit_hashmap_entry *entry = ckit_hashmap_entry_get(map, key);
    if (entry != NULL) {
        return entry->value;
    }
    return NULL;
}

const void *ckit_hashmap_get_const(const ckit_hashmap *map, const void *key) {
    CKIT_ASSERT(map != NULL, "fatal: ckit_hashmap_get_const invalid arguments");
    CKIT_ASSERT(key != NULL, "fatal: ckit_hashmap_get_const invalid arguments");

    ckit_hashmap_entry *entry = ckit_hashmap_entry_get(map, key);
    if (entry != NULL) {
        return entry->value;
    }
    return NULL;
}

void ckit_hashmap_remove(ckit_hashmap *map, const void *key) {
    CKIT_ASSERT(map != NULL, "fatal: ckit_hashmap_remove invalid arguments");
    CKIT_ASSERT(key != NULL, "fatal: ckit_hashmap_remove invalid arguments");

    size_t bucket = ckit_hash_bucket_index(key, map->key_size, map->capacity);
    ckit_linked_list_node *node = ckit_hash_bucket_remove(map->buckets[bucket], key, map->key_size,
                                                          map->key_eq, ckit_hashmap_entry_key);
    if (node != NULL) {
        ckit_hashmap_entry *entry = CKIT_CONTAINER_OF(node, ckit_hashmap_entry, node);
        ckit_dealloc(map->allocator, entry->key);
        ckit_dealloc(map->allocator, entry->value);
        ckit_dealloc(map->allocator, entry);
        map->size -= 1;
    }
}

void ckit_hashmap_free(ckit_hashmap *map) {
    CKIT_ASSERT(map != NULL, "fatal: ckit_hashmap_free invalid arguments");

    ckit_allocator *allocator = map->allocator;
    ckit_hash_buckets_free(map->buckets, map->capacity, allocator, ckit_hashmap_entry_free);
    ckit_dealloc(allocator, map);
}

size_t ckit_hashmap_size(const ckit_hashmap *map) {
    CKIT_ASSERT(map != NULL, "fatal: ckit_hashmap_size invalid arguments");

    return map->size;
}
