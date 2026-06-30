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

#include "allocators/size.h"
#include "hash/common.h"
#include "hash/fnv.h"
#include "k4c/allocators/allocator.h"
#include "k4c/allocators/utils.h"
#include "k4c/assert.h"
#include "k4c/datastruct/iterator.h"
#include "k4c/datastruct/linked_list.h"
#include "k4c/error.h"
#include "k4c/hash/hashmap.h"

typedef struct k4c_hashmap_entry {
    k4c_linked_list_node node;
    size_t hash;
    max_align_t align;
    uint8_t data[];
} k4c_hashmap_entry;

struct k4c_hashmap {
    size_t size;
    size_t key_size;
    size_t value_size;
    size_t capacity;
    k4c_hash_common_bucket *buckets;
    k4c_hashmap_key_eq_fn key_eq;
    k4c_allocator k4c_allocator;
};

typedef struct k4c_hashmap_iterator_state {
    const k4c_hashmap *map;
    size_t bucket;
    k4c_linked_list_node *node;
    k4c_hashmap_entry_view entry;
} k4c_hashmap_iterator_state;

_Static_assert(
    sizeof(k4c_hashmap_iterator_state) <= K4C_ITERATOR_STATE_SIZE,
    "k4c_hashmap_iterator_state must fit in k4c_iterator"
);

static void k4c_hashmap_entry_destroy(k4c_linked_list_node *node, k4c_allocator *k4c_allocator) {
    k4c_hashmap_entry *entry = K4C_CONTAINER_OF(node, k4c_hashmap_entry, node);
    k4c_dealloc(k4c_allocator, entry);
}

static const void *k4c_hashmap_entry_key(const k4c_linked_list_node *node) {
    const k4c_hashmap_entry *entry = K4C_CONTAINER_OF(node, k4c_hashmap_entry, node);
    return entry->data;
}

static size_t k4c_hashmap_entry_hash(const k4c_linked_list_node *node) {
    const k4c_hashmap_entry *entry = K4C_CONTAINER_OF(node, k4c_hashmap_entry, node);
    return entry->hash;
}

static void *k4c_hashmap_entry_value(const k4c_hashmap *map, k4c_hashmap_entry *entry) {
    size_t value_offset = k4c_align_up(map->key_size, K4C_MEMORY_ALIGN);
    return entry->data + value_offset;
}

static const void *k4c_hashmap_entry_value_const(
    const k4c_hashmap *map,
    const k4c_hashmap_entry *entry
) {
    size_t value_offset = k4c_align_up(map->key_size, K4C_MEMORY_ALIGN);
    return entry->data + value_offset;
}

static k4c_hashmap_entry *k4c_hashmap_entry_get(const k4c_hashmap *map, const void *key) {
    size_t hash = k4c_fnv1a_hash(key, map->key_size);
    size_t bucket = hash % map->capacity;
    k4c_linked_list_node *node = k4c_hash_common_bucket_find(
        &map->buckets[bucket],
        key,
        map->key_size,
        hash,
        map->key_eq,
        k4c_hashmap_entry_key,
        k4c_hashmap_entry_hash
    );
    if (node != NULL) {
        return K4C_CONTAINER_OF(node, k4c_hashmap_entry, node);
    }
    return NULL;
}

static k4c_hashmap_entry *k4c_hashmap_iterator_next_entry(
    k4c_hashmap_iterator_state *k4c_iterator
) {
    const k4c_hashmap *map = k4c_iterator->map;

    while (k4c_iterator->node == NULL && k4c_iterator->bucket < map->capacity) {
        k4c_iterator->node = k4c_hash_common_bucket_head(&map->buckets[k4c_iterator->bucket]);
        k4c_iterator->bucket += 1;
    }

    if (k4c_iterator->node == NULL) {
        return NULL;
    }

    k4c_linked_list_node *node = k4c_iterator->node;
    k4c_hashmap_entry *entry = K4C_CONTAINER_OF(node, k4c_hashmap_entry, node);
    k4c_iterator->node = node->next;
    return entry;
}

static const void *k4c_hashmap_iterator_next(void *context) {
    K4C_ASSERT(context != NULL, "fatal: k4c_hashmap_iterator_next invalid arguments");

    k4c_hashmap_iterator_state *k4c_iterator = context;
    const k4c_hashmap *map = k4c_iterator->map;
    k4c_hashmap_entry *entry = k4c_hashmap_iterator_next_entry(k4c_iterator);

    if (entry == NULL) {
        return NULL;
    }

    k4c_iterator->entry.key = entry->data;
    k4c_iterator->entry.value = k4c_hashmap_entry_value_const(map, entry);
    return &k4c_iterator->entry;
}

static const void *k4c_hashmap_key_iterator_next(void *context) {
    K4C_ASSERT(context != NULL, "fatal: k4c_hashmap_key_iterator_next invalid arguments");

    k4c_hashmap_iterator_state *k4c_iterator = context;
    k4c_hashmap_entry *entry = k4c_hashmap_iterator_next_entry(k4c_iterator);

    if (entry == NULL) {
        return NULL;
    }

    return entry->data;
}

static const void *k4c_hashmap_value_iterator_next(void *context) {
    K4C_ASSERT(context != NULL, "fatal: k4c_hashmap_value_iterator_next invalid arguments");

    k4c_hashmap_iterator_state *k4c_iterator = context;
    const k4c_hashmap *map = k4c_iterator->map;
    k4c_hashmap_entry *entry = k4c_hashmap_iterator_next_entry(k4c_iterator);

    if (entry == NULL) {
        return NULL;
    }

    return k4c_hashmap_entry_value_const(map, entry);
}

k4c_status k4c_hashmap_create(
    size_t key_size,
    size_t value_size,
    k4c_hashmap_key_eq_fn key_eq,
    k4c_allocator *k4c_allocator,
    k4c_hashmap **out
) {
    K4C_ASSERT(key_size > 0, "fatal: k4c_hashmap_create invalid arguments");
    K4C_ASSERT(value_size > 0, "fatal: k4c_hashmap_create invalid arguments");
    K4C_ASSERT(out != NULL, "fatal: k4c_hashmap_create invalid arguments");

    *out = NULL;

    size_t value_offset = 0;
    if (k4c_align_up_overflow(key_size, K4C_MEMORY_ALIGN, &value_offset)) {
        return K4C_STATUS_OVERFLOW;
    }
    size_t entry_payload_size = 0;
    if (k4c_size_add_overflow(value_offset, value_size, &entry_payload_size)) {
        return K4C_STATUS_OVERFLOW;
    }
    size_t max_entry_size = 0;
    if (k4c_size_add_overflow(sizeof(k4c_hashmap_entry), entry_payload_size, &max_entry_size)) {
        return K4C_STATUS_OVERFLOW;
    }
    (void)max_entry_size;

    k4c_hashmap *map = NULL;
    k4c_status st = k4c_alloc(k4c_allocator, sizeof(*map), (void **)&map);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    map->k4c_allocator = k4c_allocator_copy(k4c_allocator);
    st = k4c_hash_common_buckets_create(
        K4C_HASH_COMMON_DEFAULT_CAPACITY,
        &map->k4c_allocator,
        &map->buckets
    );
    if (st != K4C_STATUS_OK) {
        k4c_dealloc(&map->k4c_allocator, map);
        return st;
    }

    map->size = 0;
    map->key_size = key_size;
    map->value_size = value_size;
    map->capacity = K4C_HASH_COMMON_DEFAULT_CAPACITY;
    map->key_eq = key_eq;

    *out = map;
    return K4C_STATUS_OK;
}

k4c_status k4c_hashmap_reserve(k4c_hashmap *map, size_t size) {
    K4C_ASSERT(map != NULL, "fatal: k4c_hashmap_reserve invalid arguments");

    size_t new_capacity = 0;
    K4C_RETURN_IF_ERROR(k4c_hash_common_capacity_for_size(size, &new_capacity));
    if (new_capacity <= map->capacity) {
        return K4C_STATUS_OK;
    }

    k4c_hash_common_bucket *new_buckets = NULL;
    k4c_status st = k4c_hash_common_buckets_rehash(
        map->buckets,
        map->capacity,
        new_capacity,
        &map->k4c_allocator,
        k4c_hashmap_entry_hash,
        &new_buckets
    );
    if (st != K4C_STATUS_OK) {
        return st;
    }

    map->buckets = new_buckets;
    map->capacity = new_capacity;
    return K4C_STATUS_OK;
}

k4c_status k4c_hashmap_put(k4c_hashmap *map, const void *key, const void *value) {
    K4C_ASSERT(map != NULL, "fatal: k4c_hashmap_put invalid arguments");
    K4C_ASSERT(key != NULL, "fatal: k4c_hashmap_put invalid arguments");
    K4C_ASSERT(value != NULL, "fatal: k4c_hashmap_put invalid arguments");

    k4c_allocator *k4c_allocator = &map->k4c_allocator;
    size_t hash = k4c_fnv1a_hash(key, map->key_size);
    size_t bucket = hash % map->capacity;
    k4c_linked_list_node *node = k4c_hash_common_bucket_find(
        &map->buckets[bucket],
        key,
        map->key_size,
        hash,
        map->key_eq,
        k4c_hashmap_entry_key,
        k4c_hashmap_entry_hash
    );
    if (node != NULL) {
        k4c_hashmap_entry *entry = K4C_CONTAINER_OF(node, k4c_hashmap_entry, node);
        memcpy(k4c_hashmap_entry_value(map, entry), value, map->value_size);
        return K4C_STATUS_OK;
    }

    if (k4c_hash_common_should_grow(map->size, map->capacity)) {
        size_t reserve_size = 0;
        if (k4c_size_add_overflow(map->size, 1, &reserve_size)) {
            return K4C_STATUS_OVERFLOW;
        }

        K4C_RETURN_IF_ERROR(k4c_hashmap_reserve(map, reserve_size));
        bucket = hash % map->capacity;
    }

    size_t value_offset = 0;
    if (k4c_align_up_overflow(map->key_size, K4C_MEMORY_ALIGN, &value_offset)) {
        return K4C_STATUS_OVERFLOW;
    }

    size_t alloc_size = 0;
    if (k4c_size_add_overflow(value_offset, map->value_size, &alloc_size)
        || k4c_size_add_overflow(sizeof(k4c_hashmap_entry), alloc_size, &alloc_size)) {
        return K4C_STATUS_OVERFLOW;
    }

    k4c_hashmap_entry *entry = NULL;
    k4c_status st = k4c_alloc(k4c_allocator, alloc_size, (void **)&entry);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    entry->hash = hash;

    memcpy(entry->data, key, map->key_size);
    memcpy(k4c_hashmap_entry_value(map, entry), value, map->value_size);

    k4c_hash_common_bucket_pushfront(&map->buckets[bucket], &entry->node);
    map->size += 1;
    return K4C_STATUS_OK;
}

void *k4c_hashmap_get(const k4c_hashmap *map, const void *key) {
    K4C_ASSERT(map != NULL, "fatal: k4c_hashmap_get invalid arguments");
    K4C_ASSERT(key != NULL, "fatal: k4c_hashmap_get invalid arguments");

    k4c_hashmap_entry *entry = k4c_hashmap_entry_get(map, key);
    if (entry != NULL) {
        return k4c_hashmap_entry_value(map, entry);
    }
    return NULL;
}

void k4c_hashmap_remove(k4c_hashmap *map, const void *key) {
    K4C_ASSERT(map != NULL, "fatal: k4c_hashmap_remove invalid arguments");
    K4C_ASSERT(key != NULL, "fatal: k4c_hashmap_remove invalid arguments");

    k4c_allocator *k4c_allocator = &map->k4c_allocator;
    size_t hash = k4c_fnv1a_hash(key, map->key_size);
    size_t bucket = hash % map->capacity;
    k4c_linked_list_node *node = k4c_hash_common_bucket_remove(
        &map->buckets[bucket],
        key,
        map->key_size,
        hash,
        map->key_eq,
        k4c_hashmap_entry_key,
        k4c_hashmap_entry_hash
    );
    if (node != NULL) {
        k4c_hashmap_entry *entry = K4C_CONTAINER_OF(node, k4c_hashmap_entry, node);
        k4c_dealloc(k4c_allocator, entry);
        map->size -= 1;
    }
}

size_t k4c_hashmap_size(const k4c_hashmap *map) {
    K4C_ASSERT(map != NULL, "fatal: k4c_hashmap_size invalid arguments");

    return map->size;
}

k4c_iterator k4c_hashmap_get_iterator(const k4c_hashmap *map, k4c_hashmap_iterator_type type) {
    K4C_ASSERT(map != NULL, "fatal: k4c_hashmap_get_iterator invalid arguments");

    k4c_iterator_next_fn next = NULL;
    switch (type) {
        case K4C_HASHMAP_ITERATOR_ENTRY:
            next = k4c_hashmap_iterator_next;
            break;
        case K4C_HASHMAP_ITERATOR_KEY:
            next = k4c_hashmap_key_iterator_next;
            break;
        case K4C_HASHMAP_ITERATOR_VALUE:
            next = k4c_hashmap_value_iterator_next;
            break;
    }
    K4C_ASSERT(next != NULL, "fatal: k4c_hashmap_get_iterator invalid arguments");

    k4c_iterator iter = k4c_iterator_from_state(next);
    k4c_hashmap_iterator_state *state = k4c_iterator_state(&iter);
    state->map = map;
    state->bucket = 0;
    state->node = NULL;
    state->entry.key = NULL;
    state->entry.value = NULL;
    k4c_iterator_set_size_hint(&iter, map->size);
    return iter;
}

void k4c_hashmap_destroy(k4c_hashmap *map) {
    K4C_ASSERT(map != NULL, "fatal: k4c_hashmap_destroy invalid arguments");

    k4c_allocator *k4c_allocator = &map->k4c_allocator;
    k4c_hash_common_buckets_destroy(
        map->buckets,
        map->capacity,
        k4c_allocator,
        k4c_hashmap_entry_destroy
    );
    k4c_dealloc(k4c_allocator, map);
}
