/*
 * MIT License
 *
 * Copyright (c) 2026 Tommaso Bruno
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ds/hash_common.h"
#include "hash.h"
#include "vstd/assert.h"
#include "vstd/ds/hashmap.h"
#include "vstd/ds/iterator.h"
#include "vstd/ds/linked_list.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/utils.h"

typedef struct hashmap_entry {
    linked_list_node node;
    size_t hash;
    max_align_t align;
    uint8_t data[];
} hashmap_entry;

struct hashmap {
    size_t size;
    size_t key_size;
    size_t value_size;
    size_t capacity;
    hash_common_bucket *buckets;
    hashmap_key_eq_fn key_eq;
    allocator *allocator;
};

typedef struct hashmap_iterator_state {
    const hashmap *map;
    size_t bucket;
    linked_list_node *node;
    hashmap_entry_view entry;
} hashmap_iterator_state;

_Static_assert(
    sizeof(hashmap_iterator_state) <= ITERATOR_STATE_SIZE,
    "hashmap_iterator_state must fit in iterator"
);

static void hashmap_entry_destroy(linked_list_node *node, allocator *allocator) {
    hashmap_entry *entry = CONTAINER_OF(node, hashmap_entry, node);
    dealloc(allocator, entry);
}

static const void *hashmap_entry_key(const linked_list_node *node) {
    const hashmap_entry *entry = CONTAINER_OF(node, hashmap_entry, node);
    return entry->data;
}

static size_t hashmap_entry_hash(const linked_list_node *node) {
    const hashmap_entry *entry = CONTAINER_OF(node, hashmap_entry, node);
    return entry->hash;
}

static void *hashmap_entry_value(const hashmap *map, hashmap_entry *entry) {
    size_t value_offset = align_up(map->key_size, MEMORY_ALIGN);
    return entry->data + value_offset;
}

static const void *hashmap_entry_value_const(const hashmap *map, const hashmap_entry *entry) {
    size_t value_offset = align_up(map->key_size, MEMORY_ALIGN);
    return entry->data + value_offset;
}

static hashmap_entry *hashmap_entry_get(const hashmap *map, const void *key) {
    size_t hash = fnv1a_hash(key, map->key_size);
    size_t bucket = hash % map->capacity;
    linked_list_node *node = hash_common_bucket_find(
        &map->buckets[bucket],
        key,
        map->key_size,
        hash,
        map->key_eq,
        hashmap_entry_key,
        hashmap_entry_hash
    );
    if (node != NULL) {
        return CONTAINER_OF(node, hashmap_entry, node);
    }
    return NULL;
}

static hashmap_entry *hashmap_iterator_next_entry(hashmap_iterator_state *iterator) {
    const hashmap *map = iterator->map;

    while (iterator->node == NULL && iterator->bucket < map->capacity) {
        iterator->node = hash_common_bucket_head(&map->buckets[iterator->bucket]);
        iterator->bucket += 1;
    }

    if (iterator->node == NULL) {
        return NULL;
    }

    linked_list_node *node = iterator->node;
    hashmap_entry *entry = CONTAINER_OF(node, hashmap_entry, node);
    iterator->node = node->next;
    return entry;
}

static const void *hashmap_iterator_next(void *context) {
    ASSERT(context != NULL, "fatal: hashmap_iterator_next invalid arguments");

    hashmap_iterator_state *iterator = context;
    const hashmap *map = iterator->map;
    hashmap_entry *entry = hashmap_iterator_next_entry(iterator);

    if (entry == NULL) {
        return NULL;
    }

    iterator->entry.key = entry->data;
    iterator->entry.value = hashmap_entry_value_const(map, entry);
    return &iterator->entry;
}

static const void *hashmap_key_iterator_next(void *context) {
    ASSERT(context != NULL, "fatal: hashmap_key_iterator_next invalid arguments");

    hashmap_iterator_state *iterator = context;
    hashmap_entry *entry = hashmap_iterator_next_entry(iterator);

    if (entry == NULL) {
        return NULL;
    }

    return entry->data;
}

static const void *hashmap_value_iterator_next(void *context) {
    ASSERT(context != NULL, "fatal: hashmap_value_iterator_next invalid arguments");

    hashmap_iterator_state *iterator = context;
    const hashmap *map = iterator->map;
    hashmap_entry *entry = hashmap_iterator_next_entry(iterator);

    if (entry == NULL) {
        return NULL;
    }

    return hashmap_entry_value_const(map, entry);
}

status hashmap_create(
    size_t key_size,
    size_t value_size,
    hashmap_key_eq_fn key_eq,
    allocator *allocator,
    hashmap **out
) {
    ASSERT(key_size > 0, "fatal: hashmap_create invalid arguments");
    ASSERT(value_size > 0, "fatal: hashmap_create invalid arguments");
    ASSERT(out != NULL, "fatal: hashmap_create invalid arguments");

    *out = NULL;

    size_t value_offset = 0;
    if (align_up_overflow(key_size, MEMORY_ALIGN, &value_offset)) {
        return STATUS_OVERFLOW;
    }
    size_t entry_payload_size = 0;
    if (size_add_overflow(value_offset, value_size, &entry_payload_size)) {
        return STATUS_OVERFLOW;
    }
    size_t max_entry_size = 0;
    if (size_add_overflow(sizeof(hashmap_entry), entry_payload_size, &max_entry_size)) {
        return STATUS_OVERFLOW;
    }
    (void)max_entry_size;

    hashmap *map = NULL;
    status st = alloc(allocator, sizeof(*map), (void **)&map);
    if (st != STATUS_OK) {
        return st;
    }

    map->allocator = allocator;
    st = hash_common_buckets_create(HASH_COMMON_DEFAULT_CAPACITY, allocator, &map->buckets);
    if (st != STATUS_OK) {
        dealloc(allocator, map);
        return st;
    }

    map->size = 0;
    map->key_size = key_size;
    map->value_size = value_size;
    map->capacity = HASH_COMMON_DEFAULT_CAPACITY;
    map->key_eq = key_eq;

    *out = map;
    return STATUS_OK;
}

status hashmap_reserve(hashmap *map, size_t size) {
    ASSERT(map != NULL, "fatal: hashmap_reserve invalid arguments");

    size_t new_capacity = 0;
    RETURN_IF_ERROR(hash_common_capacity_for_size(size, &new_capacity));
    if (new_capacity <= map->capacity) {
        return STATUS_OK;
    }

    hash_common_bucket *new_buckets = NULL;
    status st = hash_common_buckets_rehash(
        map->buckets,
        map->capacity,
        new_capacity,
        map->allocator,
        hashmap_entry_hash,
        &new_buckets
    );
    if (st != STATUS_OK) {
        return st;
    }

    map->buckets = new_buckets;
    map->capacity = new_capacity;
    return STATUS_OK;
}

status hashmap_put(hashmap *map, const void *key, const void *value) {
    ASSERT(map != NULL, "fatal: hashmap_put invalid arguments");
    ASSERT(key != NULL, "fatal: hashmap_put invalid arguments");
    ASSERT(value != NULL, "fatal: hashmap_put invalid arguments");

    allocator *allocator = map->allocator;
    size_t hash = fnv1a_hash(key, map->key_size);
    size_t bucket = hash % map->capacity;
    linked_list_node *node = hash_common_bucket_find(
        &map->buckets[bucket],
        key,
        map->key_size,
        hash,
        map->key_eq,
        hashmap_entry_key,
        hashmap_entry_hash
    );
    if (node != NULL) {
        hashmap_entry *entry = CONTAINER_OF(node, hashmap_entry, node);
        memcpy(hashmap_entry_value(map, entry), value, map->value_size);
        return STATUS_OK;
    }

    if (hash_common_should_grow(map->size, map->capacity)) {
        size_t reserve_size = 0;
        if (size_add_overflow(map->size, 1, &reserve_size)) {
            return STATUS_OVERFLOW;
        }

        RETURN_IF_ERROR(hashmap_reserve(map, reserve_size));
        bucket = hash % map->capacity;
    }

    size_t value_offset = 0;
    if (align_up_overflow(map->key_size, MEMORY_ALIGN, &value_offset)) {
        return STATUS_OVERFLOW;
    }

    size_t alloc_size = 0;
    if (size_add_overflow(value_offset, map->value_size, &alloc_size)
        || size_add_overflow(sizeof(hashmap_entry), alloc_size, &alloc_size)) {
        return STATUS_OVERFLOW;
    }

    hashmap_entry *entry = NULL;
    status st = alloc(allocator, alloc_size, (void **)&entry);
    if (st != STATUS_OK) {
        return st;
    }

    entry->hash = hash;

    memcpy(entry->data, key, map->key_size);
    memcpy(hashmap_entry_value(map, entry), value, map->value_size);

    hash_common_bucket_pushfront(&map->buckets[bucket], &entry->node);
    map->size += 1;
    return STATUS_OK;
}

void *hashmap_get(hashmap *map, const void *key) {
    ASSERT(map != NULL, "fatal: hashmap_get invalid arguments");
    ASSERT(key != NULL, "fatal: hashmap_get invalid arguments");

    hashmap_entry *entry = hashmap_entry_get(map, key);
    if (entry != NULL) {
        return hashmap_entry_value(map, entry);
    }
    return NULL;
}

const void *hashmap_get_const(const hashmap *map, const void *key) {
    ASSERT(map != NULL, "fatal: hashmap_get_const invalid arguments");
    ASSERT(key != NULL, "fatal: hashmap_get_const invalid arguments");

    hashmap_entry *entry = hashmap_entry_get(map, key);
    if (entry != NULL) {
        return hashmap_entry_value_const(map, entry);
    }
    return NULL;
}

void hashmap_remove(hashmap *map, const void *key) {
    ASSERT(map != NULL, "fatal: hashmap_remove invalid arguments");
    ASSERT(key != NULL, "fatal: hashmap_remove invalid arguments");

    allocator *allocator = map->allocator;
    size_t hash = fnv1a_hash(key, map->key_size);
    size_t bucket = hash % map->capacity;
    linked_list_node *node = hash_common_bucket_remove(
        &map->buckets[bucket],
        key,
        map->key_size,
        hash,
        map->key_eq,
        hashmap_entry_key,
        hashmap_entry_hash
    );
    if (node != NULL) {
        hashmap_entry *entry = CONTAINER_OF(node, hashmap_entry, node);
        dealloc(allocator, entry);
        map->size -= 1;
    }
}

size_t hashmap_size(const hashmap *map) {
    ASSERT(map != NULL, "fatal: hashmap_size invalid arguments");

    return map->size;
}

iterator hashmap_get_iterator(const hashmap *map, hashmap_iterator_type type) {
    ASSERT(map != NULL, "fatal: hashmap_get_iterator invalid arguments");

    iterator_next_fn next = NULL;
    switch (type) {
        case HASHMAP_ITERATOR_ENTRY:
            next = hashmap_iterator_next;
            break;
        case HASHMAP_ITERATOR_KEY:
            next = hashmap_key_iterator_next;
            break;
        case HASHMAP_ITERATOR_VALUE:
            next = hashmap_value_iterator_next;
            break;
    }
    ASSERT(next != NULL, "fatal: hashmap_get_iterator invalid arguments");

    iterator iter = iterator_from_state(next);
    hashmap_iterator_state *state = iterator_state(&iter);
    state->map = map;
    state->bucket = 0;
    state->node = NULL;
    state->entry.key = NULL;
    state->entry.value = NULL;
    iterator_set_size_hint(&iter, map->size);
    return iter;
}

void hashmap_destroy(hashmap *map) {
    ASSERT(map != NULL, "fatal: hashmap_destroy invalid arguments");

    allocator *allocator = map->allocator;
    hash_common_buckets_destroy(map->buckets, map->capacity, allocator, hashmap_entry_destroy);
    dealloc(allocator, map);
}
