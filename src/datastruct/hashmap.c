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

#include "datastruct/hash_common.h"
#include "hash.h"
#include "vstd/assert.h"
#include "vstd/datastruct/hashmap.h"
#include "vstd/datastruct/iterator.h"
#include "vstd/datastruct/linked_list.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/utils.h"

typedef struct vs_hashmap_entry {
    vs_linked_list_node node;
    size_t hash;
    max_align_t align;
    uint8_t data[];
} vs_hashmap_entry;

struct vs_hashmap {
    size_t size;
    size_t key_size;
    size_t value_size;
    size_t capacity;
    vs_hash_common_bucket *buckets;
    vs_hashmap_key_eq_fn key_eq;
    vs_allocator *allocator;
};

static void vs_hashmap_entry_destroy(vs_linked_list_node *node, vs_allocator *allocator) {
    vs_hashmap_entry *entry = VS_CONTAINER_OF(node, vs_hashmap_entry, node);
    vs_dealloc(allocator, entry);
}

static const void *vs_hashmap_entry_key(const vs_linked_list_node *node) {
    const vs_hashmap_entry *entry = VS_CONTAINER_OF(node, vs_hashmap_entry, node);
    return entry->data;
}

static size_t vs_hashmap_entry_hash(const vs_linked_list_node *node) {
    const vs_hashmap_entry *entry = VS_CONTAINER_OF(node, vs_hashmap_entry, node);
    return entry->hash;
}

static void *vs_hashmap_entry_value(const vs_hashmap *map, vs_hashmap_entry *entry) {
    size_t value_offset = vs_align_up(map->key_size, VS_MEMORY_ALIGN);
    return entry->data + value_offset;
}

static const void *vs_hashmap_entry_value_const(
    const vs_hashmap *map,
    const vs_hashmap_entry *entry
) {
    size_t value_offset = vs_align_up(map->key_size, VS_MEMORY_ALIGN);
    return entry->data + value_offset;
}

static vs_hashmap_entry *vs_hashmap_entry_get(const vs_hashmap *map, const void *key) {
    size_t hash = vs_fnv1a_hash(key, map->key_size);
    size_t bucket = vs_hash_common_bucket_index(hash, map->capacity);
    vs_linked_list_node *node = vs_hash_common_bucket_find(
        &map->buckets[bucket],
        key,
        map->key_size,
        hash,
        map->key_eq,
        vs_hashmap_entry_key,
        vs_hashmap_entry_hash
    );
    if (node != NULL) {
        return VS_CONTAINER_OF(node, vs_hashmap_entry, node);
    }
    return NULL;
}

vs_hashmap *vs_hashmap_create(
    size_t key_size,
    size_t value_size,
    vs_hashmap_key_eq_fn key_eq,
    vs_allocator *allocator
) {
    VSTD_ASSERT(key_size > 0, "fatal: vs_hashmap_create invalid arguments");
    VSTD_ASSERT(value_size > 0, "fatal: vs_hashmap_create invalid arguments");

    vs_hashmap *map = vs_malloc(allocator, sizeof(vs_hashmap));
    map->allocator = allocator;
    map->buckets = vs_hash_common_buckets_create(VS_HASH_COMMON_DEFAULT_CAPACITY, allocator);

    map->size = 0;
    map->key_size = key_size;
    map->value_size = value_size;
    map->capacity = VS_HASH_COMMON_DEFAULT_CAPACITY;
    map->key_eq = key_eq;

    return map;
}

void vs_hashmap_reserve(vs_hashmap *map, size_t size) {
    VSTD_ASSERT(map != NULL, "fatal: vs_hashmap_reserve invalid arguments");

    size_t new_capacity = vs_hash_common_capacity_for_size(size);
    if (new_capacity <= map->capacity) {
        return;
    }

    map->buckets = vs_hash_common_buckets_rehash(
        map->buckets,
        map->capacity,
        new_capacity,
        map->allocator,
        vs_hashmap_entry_hash
    );
    map->capacity = new_capacity;
}

void vs_hashmap_put(vs_hashmap *map, const void *key, const void *value) {
    VSTD_ASSERT(map != NULL, "fatal: vs_hashmap_put invalid arguments");
    VSTD_ASSERT(key != NULL, "fatal: vs_hashmap_put invalid arguments");
    VSTD_ASSERT(value != NULL, "fatal: vs_hashmap_put invalid arguments");

    vs_allocator *allocator = map->allocator;
    size_t hash = vs_fnv1a_hash(key, map->key_size);
    size_t bucket = vs_hash_common_bucket_index(hash, map->capacity);
    vs_linked_list_node *node = vs_hash_common_bucket_find(
        &map->buckets[bucket],
        key,
        map->key_size,
        hash,
        map->key_eq,
        vs_hashmap_entry_key,
        vs_hashmap_entry_hash
    );
    if (node != NULL) {
        vs_hashmap_entry *entry = VS_CONTAINER_OF(node, vs_hashmap_entry, node);
        memcpy(vs_hashmap_entry_value(map, entry), value, map->value_size);
        return;
    }

    if (vs_hash_common_should_grow(map->size, map->capacity)) {
        vs_hashmap_reserve(map, map->size + 1);
        bucket = vs_hash_common_bucket_index(hash, map->capacity);
    }

    size_t value_offset = vs_align_up(map->key_size, VS_MEMORY_ALIGN);
    size_t alloc_size = sizeof(vs_hashmap_entry) + value_offset + map->value_size;
    vs_hashmap_entry *entry = vs_malloc(allocator, alloc_size);
    entry->hash = hash;

    memcpy(entry->data, key, map->key_size);
    memcpy(vs_hashmap_entry_value(map, entry), value, map->value_size);

    vs_hash_common_bucket_pushfront(&map->buckets[bucket], &entry->node);
    map->size += 1;
}

void *vs_hashmap_get(vs_hashmap *map, const void *key) {
    VSTD_ASSERT(map != NULL, "fatal: vs_hashmap_get invalid arguments");
    VSTD_ASSERT(key != NULL, "fatal: vs_hashmap_get invalid arguments");

    vs_hashmap_entry *entry = vs_hashmap_entry_get(map, key);
    if (entry != NULL) {
        return vs_hashmap_entry_value(map, entry);
    }
    return NULL;
}

const void *vs_hashmap_get_const(const vs_hashmap *map, const void *key) {
    VSTD_ASSERT(map != NULL, "fatal: vs_hashmap_get_const invalid arguments");
    VSTD_ASSERT(key != NULL, "fatal: vs_hashmap_get_const invalid arguments");

    vs_hashmap_entry *entry = vs_hashmap_entry_get(map, key);
    if (entry != NULL) {
        return vs_hashmap_entry_value_const(map, entry);
    }
    return NULL;
}

void vs_hashmap_remove(vs_hashmap *map, const void *key) {
    VSTD_ASSERT(map != NULL, "fatal: vs_hashmap_remove invalid arguments");
    VSTD_ASSERT(key != NULL, "fatal: vs_hashmap_remove invalid arguments");

    vs_allocator *allocator = map->allocator;
    size_t hash = vs_fnv1a_hash(key, map->key_size);
    size_t bucket = vs_hash_common_bucket_index(hash, map->capacity);
    vs_linked_list_node *node = vs_hash_common_bucket_remove(
        &map->buckets[bucket],
        key,
        map->key_size,
        hash,
        map->key_eq,
        vs_hashmap_entry_key,
        vs_hashmap_entry_hash
    );
    if (node != NULL) {
        vs_hashmap_entry *entry = VS_CONTAINER_OF(node, vs_hashmap_entry, node);
        vs_dealloc(allocator, entry);
        map->size -= 1;
    }
}

size_t vs_hashmap_size(const vs_hashmap *map) {
    VSTD_ASSERT(map != NULL, "fatal: vs_hashmap_size invalid arguments");

    return map->size;
}

typedef struct vs_hashmap_iterator_state {
    const vs_hashmap *map;
    size_t bucket;
    vs_linked_list_node *node;
    vs_hashmap_entry_view entry;
} vs_hashmap_iterator_state;

_Static_assert(
    sizeof(vs_hashmap_iterator_state) <= VS_ITERATOR_STATE_SIZE,
    "vs_hashmap_iterator_state must fit in vs_iterator"
);

static vs_hashmap_entry *vs_hashmap_iterator_next_entry(vs_hashmap_iterator_state *iterator) {
    const vs_hashmap *map = iterator->map;

    while (iterator->node == NULL && iterator->bucket < map->capacity) {
        iterator->node = vs_hash_common_bucket_head(&map->buckets[iterator->bucket]);
        iterator->bucket += 1;
    }

    if (iterator->node == NULL) {
        return NULL;
    }

    vs_linked_list_node *node = iterator->node;
    vs_hashmap_entry *entry = VS_CONTAINER_OF(node, vs_hashmap_entry, node);
    iterator->node = node->next;
    return entry;
}

static const void *vs_hashmap_iterator_next(void *context) {
    VSTD_ASSERT(context != NULL, "fatal: vs_hashmap_iterator_next invalid arguments");

    vs_hashmap_iterator_state *iterator = context;
    const vs_hashmap *map = iterator->map;
    vs_hashmap_entry *entry = vs_hashmap_iterator_next_entry(iterator);

    if (entry == NULL) {
        return NULL;
    }

    iterator->entry.key = entry->data;
    iterator->entry.value = vs_hashmap_entry_value_const(map, entry);
    return &iterator->entry;
}

static const void *vs_hashmap_key_iterator_next(void *context) {
    VSTD_ASSERT(context != NULL, "fatal: vs_hashmap_key_iterator_next invalid arguments");

    vs_hashmap_iterator_state *iterator = context;
    vs_hashmap_entry *entry = vs_hashmap_iterator_next_entry(iterator);

    if (entry == NULL) {
        return NULL;
    }

    return entry->data;
}

static const void *vs_hashmap_value_iterator_next(void *context) {
    VSTD_ASSERT(context != NULL, "fatal: vs_hashmap_value_iterator_next invalid arguments");

    vs_hashmap_iterator_state *iterator = context;
    const vs_hashmap *map = iterator->map;
    vs_hashmap_entry *entry = vs_hashmap_iterator_next_entry(iterator);

    if (entry == NULL) {
        return NULL;
    }

    return vs_hashmap_entry_value_const(map, entry);
}

vs_iterator vs_hashmap_get_iterator(const vs_hashmap *map, vs_hashmap_iterator_type type) {
    VSTD_ASSERT(map != NULL, "fatal: vs_hashmap_get_iterator invalid arguments");

    vs_iterator_next_fn next = NULL;
    switch (type) {
        case VS_HASHMAP_ITERATOR_ENTRY:
            next = vs_hashmap_iterator_next;
            break;
        case VS_HASHMAP_ITERATOR_KEY:
            next = vs_hashmap_key_iterator_next;
            break;
        case VS_HASHMAP_ITERATOR_VALUE:
            next = vs_hashmap_value_iterator_next;
            break;
    }
    VSTD_ASSERT(next != NULL, "fatal: vs_hashmap_get_iterator invalid arguments");

    vs_iterator iter = vs_iterator_from_state(next);
    vs_hashmap_iterator_state *state = vs_iterator_state(&iter);
    state->map = map;
    state->bucket = 0;
    state->node = NULL;
    state->entry.key = NULL;
    state->entry.value = NULL;
    vs_iterator_set_size_hint(&iter, map->size);
    return iter;
}

void vs_hashmap_destroy(vs_hashmap *map) {
    VSTD_ASSERT(map != NULL, "fatal: vs_hashmap_destroy invalid arguments");

    vs_allocator *allocator = map->allocator;
    vs_hash_common_buckets_destroy(
        map->buckets,
        map->capacity,
        allocator,
        vs_hashmap_entry_destroy
    );
    vs_dealloc(allocator, map);
}
