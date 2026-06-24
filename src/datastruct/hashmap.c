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
#include "vstd/assert.h"
#include "vstd/datastruct/hashmap.h"
#include "vstd/datastruct/iterator.h"
#include "vstd/datastruct/linked_list.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/utils.h"

typedef struct vs_hashmap_entry {
    vs_linked_list_node node;
    max_align_t align;
    uint8_t data[];
} vs_hashmap_entry;

struct vs_hashmap {
    size_t size;
    size_t key_size;
    size_t value_size;
    size_t capacity;
    vs_linked_list **buckets;
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
    size_t bucket = vs_hash_common_bucket_index(key, map->key_size, map->capacity);
    vs_linked_list_node *node = vs_hash_common_bucket_find(
        map->buckets[bucket],
        key,
        map->key_size,
        map->key_eq,
        vs_hashmap_entry_key
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

void vs_hashmap_put(vs_hashmap *map, const void *key, const void *value) {
    VSTD_ASSERT(map != NULL, "fatal: vs_hashmap_put invalid arguments");
    VSTD_ASSERT(key != NULL, "fatal: vs_hashmap_put invalid arguments");
    VSTD_ASSERT(value != NULL, "fatal: vs_hashmap_put invalid arguments");

    vs_allocator *allocator = map->allocator;
    size_t bucket = vs_hash_common_bucket_index(key, map->key_size, map->capacity);
    vs_linked_list_node *node = vs_hash_common_bucket_find(
        map->buckets[bucket],
        key,
        map->key_size,
        map->key_eq,
        vs_hashmap_entry_key
    );
    if (node != NULL) {
        vs_hashmap_entry *entry = VS_CONTAINER_OF(node, vs_hashmap_entry, node);
        memcpy(vs_hashmap_entry_value(map, entry), value, map->value_size);
        return;
    }

    if (vs_hash_common_should_grow(map->size, map->capacity)) {
        size_t new_capacity = map->capacity * 2;
        map->buckets = vs_hash_common_buckets_rehash(
            map->buckets,
            map->capacity,
            new_capacity,
            map->key_size,
            allocator,
            vs_hashmap_entry_key
        );
        map->capacity = new_capacity;
        bucket = vs_hash_common_bucket_index(key, map->key_size, map->capacity);
    }

    size_t value_offset = vs_align_up(map->key_size, VS_MEMORY_ALIGN);
    size_t alloc_size = sizeof(vs_hashmap_entry) + value_offset + map->value_size;
    vs_hashmap_entry *entry = vs_malloc(allocator, alloc_size);

    memcpy(entry->data, key, map->key_size);
    memcpy(vs_hashmap_entry_value(map, entry), value, map->value_size);

    vs_linked_list_pushfront(map->buckets[bucket], &entry->node);
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
    size_t bucket = vs_hash_common_bucket_index(key, map->key_size, map->capacity);
    vs_linked_list_node *node = vs_hash_common_bucket_remove(
        map->buckets[bucket],
        key,
        map->key_size,
        map->key_eq,
        vs_hashmap_entry_key
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

static vs_hashmap_entry *vs_hashmap_iterator_next_entry(vs_hashmap_iterator_state *state) {
    const vs_hashmap *map = state->map;

    while (state->node == NULL && state->bucket < map->capacity) {
        state->node = vs_linked_list_head(map->buckets[state->bucket]);
        state->bucket += 1;
    }

    if (state->node == NULL) {
        return NULL;
    }

    vs_linked_list_node *node = state->node;
    vs_hashmap_entry *entry = VS_CONTAINER_OF(node, vs_hashmap_entry, node);
    state->node = node->next;
    return entry;
}

static const void *vs_hashmap_iterator_next(void *context) {
    vs_hashmap_iterator_state *state = context;
    const vs_hashmap *map = state->map;
    vs_hashmap_entry *entry = vs_hashmap_iterator_next_entry(state);

    if (entry == NULL) {
        return NULL;
    }

    state->entry.key = entry->data;
    state->entry.value = vs_hashmap_entry_value_const(map, entry);
    return &state->entry;
}

static const void *vs_hashmap_key_iterator_next(void *context) {
    vs_hashmap_iterator_state *state = context;
    vs_hashmap_entry *entry = vs_hashmap_iterator_next_entry(state);

    if (entry == NULL) {
        return NULL;
    }

    return entry->data;
}

static const void *vs_hashmap_value_iterator_next(void *context) {
    vs_hashmap_iterator_state *state = context;
    const vs_hashmap *map = state->map;
    vs_hashmap_entry *entry = vs_hashmap_iterator_next_entry(state);

    if (entry == NULL) {
        return NULL;
    }

    return vs_hashmap_entry_value_const(map, entry);
}

vs_iterator vs_hashmap_iterator(vs_hashmap_iterator_state *state, const vs_hashmap *map) {
    VSTD_ASSERT(state != NULL, "fatal: vs_hashmap_iterator invalid arguments");
    VSTD_ASSERT(map != NULL, "fatal: vs_hashmap_iterator invalid arguments");

    state->map = map;
    state->bucket = 0;
    state->node = NULL;
    state->entry.key = NULL;
    state->entry.value = NULL;
    return vs_iterator_from_callback(state, vs_hashmap_iterator_next);
}

vs_iterator vs_hashmap_key_iterator(vs_hashmap_iterator_state *state, const vs_hashmap *map) {
    VSTD_ASSERT(state != NULL, "fatal: vs_hashmap_key_iterator invalid arguments");
    VSTD_ASSERT(map != NULL, "fatal: vs_hashmap_key_iterator invalid arguments");

    state->map = map;
    state->bucket = 0;
    state->node = NULL;
    state->entry.key = NULL;
    state->entry.value = NULL;
    return vs_iterator_from_callback(state, vs_hashmap_key_iterator_next);
}

vs_iterator vs_hashmap_value_iterator(vs_hashmap_iterator_state *state, const vs_hashmap *map) {
    VSTD_ASSERT(state != NULL, "fatal: vs_hashmap_value_iterator invalid arguments");
    VSTD_ASSERT(map != NULL, "fatal: vs_hashmap_value_iterator invalid arguments");

    state->map = map;
    state->bucket = 0;
    state->node = NULL;
    state->entry.key = NULL;
    state->entry.value = NULL;
    return vs_iterator_from_callback(state, vs_hashmap_value_iterator_next);
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
