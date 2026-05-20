#include <string.h>
#include <stdlib.h>

#include "ckit/datastruct/hashmap.h"
#include "crypto/fnv1a.h"
#include "ckit/mem/allocators/allocator.h"

#define CKIT_HASHMAP_DEFAULT_CAPACITY 16U
#define CKIT_HASHMAP_MAX_LOAD_FACTOR_NUM 3U
#define CKIT_HASHMAP_MAX_LOAD_FACTOR_DEN 4U

static void *ckit_hashmap_alloc(const ckit_hashmap *map, size_t size) {
    if (map->allocator == NULL || map->allocator->alloc == NULL) {
        return ckit_malloc(size);
    }
    return map->allocator->alloc(map->allocator->ctx, size);
}

static void ckit_hashmap_dealloc(const ckit_hashmap *map, void *ptr) {
    if (map->allocator == NULL || map->allocator->dealloc == NULL) {
        free(ptr);
        return;
    }
    map->allocator->dealloc(map->allocator->ctx, ptr);
}

static size_t ckit_hashmap_bucket_index(const ckit_hashmap *map, const void *key) {
    size_t hash = ckit_internal_fnv1a_hash(key, map->key_size);
    return hash % map->capacity;
}

static bool ckit_hashmap_should_grow(const ckit_hashmap *map) {
    return (map->size + 1U) * CKIT_HASHMAP_MAX_LOAD_FACTOR_DEN >
           map->capacity * CKIT_HASHMAP_MAX_LOAD_FACTOR_NUM;
}

static ckit_status ckit_hashmap_rehash(ckit_hashmap *map, size_t new_capacity) {
    ckit_hashmap_entry **new_buckets =
        (ckit_hashmap_entry **)ckit_hashmap_alloc(map, sizeof(*new_buckets) * new_capacity);
    if (new_buckets == NULL) {
        return CKIT_ERR_RANGE;
    }
    memset((void *)new_buckets, 0, sizeof(*new_buckets) * new_capacity);

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

    ckit_hashmap_dealloc(map, (void *)map->buckets);
    map->buckets = new_buckets;
    map->capacity = new_capacity;

    return CKIT_OK;
}

ckit_status ckit_hashmap_init(ckit_hashmap *map, size_t key_size, size_t value_size,
                              ckit_hashmap_key_eq_fn key_eq, ckit_allocator *allocator) {
    if (map == NULL || key_eq == NULL) {
        return CKIT_ERR_NULL;
    }
    if (key_size == 0U || value_size == 0U) {
        return CKIT_ERR_RANGE;
    }

    map->allocator = allocator;

    map->buckets = (ckit_hashmap_entry **)ckit_hashmap_alloc(
        map, sizeof(*map->buckets) * CKIT_HASHMAP_DEFAULT_CAPACITY);
    if (map->buckets == NULL) {
        return CKIT_ERR_RANGE;
    }
    memset((void *)map->buckets, 0, sizeof(*map->buckets) * CKIT_HASHMAP_DEFAULT_CAPACITY);

    map->size = 0U;
    map->key_size = key_size;
    map->value_size = value_size;
    map->capacity = CKIT_HASHMAP_DEFAULT_CAPACITY;
    map->key_eq = key_eq;

    return CKIT_OK;
}

ckit_status ckit_hashmap_put(ckit_hashmap *map, const void *key, const void *value) {
    if (map == NULL || key == NULL || value == NULL) {
        return CKIT_ERR_NULL;
    }

    size_t bucket = ckit_hashmap_bucket_index(map, key);
    ckit_hashmap_entry *curr = map->buckets[bucket];

    while (curr != NULL) {
        if (map->key_eq(curr->key, key, map->key_size)) {
            memcpy(curr->value, value, map->value_size);
            return CKIT_OK;
        }
        curr = curr->next;
    }

    if (ckit_hashmap_should_grow(map)) {
        ckit_status status = ckit_hashmap_rehash(map, map->capacity * 2U);
        if (status != CKIT_OK) {
            return status;
        }
        bucket = ckit_hashmap_bucket_index(map, key);
    }

    ckit_hashmap_entry *entry = ckit_hashmap_alloc(map, sizeof(*entry));
    if (entry == NULL) {
        return CKIT_ERR_RANGE;
    }

    entry->key = ckit_hashmap_alloc(map, map->key_size);
    entry->value = ckit_hashmap_alloc(map, map->value_size);
    if (entry->key == NULL || entry->value == NULL) {
        ckit_hashmap_dealloc(map, entry->key);
        ckit_hashmap_dealloc(map, entry->value);
        ckit_hashmap_dealloc(map, entry);
        return CKIT_ERR_RANGE;
    }

    memcpy(entry->key, key, map->key_size);
    memcpy(entry->value, value, map->value_size);

    entry->next = map->buckets[bucket];
    map->buckets[bucket] = entry;
    map->size += 1U;

    return CKIT_OK;
}

ckit_status ckit_hashmap_get(const ckit_hashmap *map, const void *key, void *out_value) {
    if (map == NULL || key == NULL || out_value == NULL) {
        return CKIT_ERR_NULL;
    }

    size_t bucket = ckit_hashmap_bucket_index(map, key);
    ckit_hashmap_entry *curr = map->buckets[bucket];

    while (curr != NULL) {
        if (map->key_eq(curr->key, key, map->key_size)) {
            memcpy(out_value, curr->value, map->value_size);
            return CKIT_OK;
        }
        curr = curr->next;
    }

    return CKIT_ERR_NOT_FOUND;
}

ckit_status ckit_hashmap_remove(ckit_hashmap *map, const void *key) {
    if (map == NULL || key == NULL) {
        return CKIT_ERR_NULL;
    }

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

            ckit_hashmap_dealloc(map, curr->key);
            ckit_hashmap_dealloc(map, curr->value);
            ckit_hashmap_dealloc(map, curr);
            map->size -= 1U;
            return CKIT_OK;
        }

        prev = curr;
        curr = curr->next;
    }

    return CKIT_ERR_NOT_FOUND;
}

ckit_status ckit_hashmap_free(ckit_hashmap *map) {
    if (map == NULL) {
        return CKIT_ERR_NULL;
    }

    if (map->buckets != NULL) {
        for (size_t i = 0; i < map->capacity; i++) {
            ckit_hashmap_entry *curr = map->buckets[i];
            while (curr != NULL) {
                ckit_hashmap_entry *next = curr->next;
                ckit_hashmap_dealloc(map, curr->key);
                ckit_hashmap_dealloc(map, curr->value);
                ckit_hashmap_dealloc(map, curr);
                curr = next;
            }
        }
    }

    ckit_hashmap_dealloc(map, (void *)map->buckets);
    map->buckets = NULL;
    map->size = 0U;
    map->key_size = 0U;
    map->value_size = 0U;
    map->capacity = 0U;
    map->key_eq = NULL;
    map->allocator = NULL;

    return CKIT_OK;
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
