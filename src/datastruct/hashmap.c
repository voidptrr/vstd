#include <string.h>

#include "ckit/common/panic.h"
#include "ckit/datastruct/hashmap.h"
#include "ckit/memory/allocators/allocator.h"
#include "crypto/fnv1a.h"

#define CKIT_HASHMAP_DEFAULT_CAPACITY 16U
#define CKIT_HASHMAP_MAX_LOAD_FACTOR_NUM 3U
#define CKIT_HASHMAP_MAX_LOAD_FACTOR_DEN 4U

typedef struct ckit_hashmap_entry {
    void *key;
    void *value;
    struct ckit_hashmap_entry *next;
} ckit_hashmap_entry;

struct ckit_hashmap {
    size_t size;
    size_t key_size;
    size_t value_size;
    size_t capacity;
    ckit_hashmap_entry **buckets;
    ckit_hashmap_key_eq_fn key_eq;
    ckit_allocator *allocator;
};

static size_t ckit_hashmap_bucket_index(const ckit_hashmap *map, const void *key) {
    size_t hash = ckit_internal_fnv1a_hash(key, map->key_size);
    return hash % map->capacity;
}

static bool ckit_hashmap_should_grow(const ckit_hashmap *map) {
    return (map->size + 1U) * CKIT_HASHMAP_MAX_LOAD_FACTOR_DEN >
           map->capacity * CKIT_HASHMAP_MAX_LOAD_FACTOR_NUM;
}

static void ckit_hashmap_rehash(ckit_hashmap *map, size_t new_capacity) {
    size_t alloc_size = sizeof(*map->buckets) * new_capacity;
    ckit_hashmap_entry **new_buckets =
        (ckit_hashmap_entry **)ckit_malloc(map->allocator, alloc_size);
    memset((void *)new_buckets, 0, alloc_size);

    for (size_t i = 0U; i < map->capacity; i++) {
        ckit_hashmap_entry *curr = map->buckets[i];
        while (curr != NULL) {
            ckit_hashmap_entry *next = curr->next;
            size_t hash = ckit_internal_fnv1a_hash(curr->key, map->key_size);
            size_t bucket = hash % new_capacity;

            curr->next = new_buckets[bucket];
            new_buckets[bucket] = curr;
            curr = next;
        }
    }

    ckit_dealloc(map->allocator, (void *)map->buckets);
    map->buckets = new_buckets;
    map->capacity = new_capacity;
}

ckit_hashmap *ckit_hashmap_init(size_t key_size, size_t value_size, ckit_hashmap_key_eq_fn key_eq,
                                ckit_allocator *allocator) {
    CKIT_ASSERT(key_eq != NULL, "fatal: ckit_hashmap_init invalid arguments");
    CKIT_ASSERT(key_size > 0U, "fatal: ckit_hashmap_init invalid arguments");
    CKIT_ASSERT(value_size > 0U, "fatal: ckit_hashmap_init invalid arguments");

    ckit_hashmap *map = ckit_malloc(allocator, sizeof(*map));
    map->allocator = allocator;
    size_t alloc_size = sizeof(*map->buckets) * CKIT_HASHMAP_DEFAULT_CAPACITY;
    map->buckets = (ckit_hashmap_entry **)ckit_malloc(allocator, alloc_size);
    memset((void *)map->buckets, 0, alloc_size);

    map->size = 0U;
    map->key_size = key_size;
    map->value_size = value_size;
    map->capacity = CKIT_HASHMAP_DEFAULT_CAPACITY;
    map->key_eq = key_eq;

    return map;
}

void ckit_hashmap_put(ckit_hashmap *map, const void *key, const void *value) {
    CKIT_ASSERT(map != NULL, "fatal: ckit_hashmap_put invalid arguments");
    CKIT_ASSERT(key != NULL, "fatal: ckit_hashmap_put invalid arguments");
    CKIT_ASSERT(value != NULL, "fatal: ckit_hashmap_put invalid arguments");

    size_t bucket = ckit_hashmap_bucket_index(map, key);
    ckit_hashmap_entry *curr = map->buckets[bucket];

    while (curr != NULL) {
        if (map->key_eq(curr->key, key, map->key_size)) {
            memcpy(curr->value, value, map->value_size);
            return;
        }
        curr = curr->next;
    }

    if (ckit_hashmap_should_grow(map)) {
        ckit_hashmap_rehash(map, map->capacity * 2U);
        bucket = ckit_hashmap_bucket_index(map, key);
    }

    ckit_hashmap_entry *entry = ckit_malloc(map->allocator, sizeof(*entry));
    entry->key = ckit_malloc(map->allocator, map->key_size);
    entry->value = ckit_malloc(map->allocator, map->value_size);

    memcpy(entry->key, key, map->key_size);
    memcpy(entry->value, value, map->value_size);

    entry->next = map->buckets[bucket];
    map->buckets[bucket] = entry;
    map->size += 1U;
}

const void *ckit_hashmap_get(const ckit_hashmap *map, const void *key) {
    CKIT_ASSERT(map != NULL, "fatal: ckit_hashmap_get invalid arguments");
    CKIT_ASSERT(key != NULL, "fatal: ckit_hashmap_get invalid arguments");

    size_t bucket = ckit_hashmap_bucket_index(map, key);
    ckit_hashmap_entry *curr = map->buckets[bucket];
    while (curr != NULL) {
        if (map->key_eq(curr->key, key, map->key_size)) {
            return curr->value;
        }
        curr = curr->next;
    }
    return NULL;
}

void *ckit_hashmap_remove(ckit_hashmap *map, const void *key) {
    CKIT_ASSERT(map != NULL, "fatal: ckit_hashmap_remove invalid arguments");
    CKIT_ASSERT(key != NULL, "fatal: ckit_hashmap_remove invalid arguments");

    size_t bucket = ckit_hashmap_bucket_index(map, key);
    ckit_hashmap_entry *prev = NULL;
    ckit_hashmap_entry *curr = map->buckets[bucket];

    while (curr != NULL) {
        if (map->key_eq(curr->key, key, map->key_size)) {
            if (prev == NULL) {
                map->buckets[bucket] = curr->next;
            } else {
                prev->next = curr->next;
            }

            void *value = curr->value;
            ckit_dealloc(map->allocator, curr->key);
            ckit_dealloc(map->allocator, curr);
            map->size -= 1U;
            return value;
        }
        prev = curr;
        curr = curr->next;
    }

    return NULL;
}

void ckit_hashmap_free(ckit_hashmap *map) {
    CKIT_ASSERT(map != NULL, "fatal: ckit_hashmap_free invalid arguments");

    ckit_allocator *allocator = map->allocator;
    if (map->buckets) {
        for (size_t i = 0U; i < map->capacity; i++) {
            ckit_hashmap_entry *curr = map->buckets[i];
            while (curr != NULL) {
                ckit_hashmap_entry *next = curr->next;
                ckit_dealloc(map->allocator, curr->key);
                ckit_dealloc(map->allocator, curr->value);
                ckit_dealloc(map->allocator, curr);
                curr = next;
            }
        }
    }

    ckit_dealloc(map->allocator, (void *)map->buckets);
    ckit_dealloc(allocator, map);
}

size_t ckit_hashmap_size(const ckit_hashmap *map) {
    if (map == NULL) {
        return 0U;
    }
    return map->size;
}

bool ckit_hashmap_is_empty(const ckit_hashmap *map) {
    return ckit_hashmap_size(map) == 0U;
}
