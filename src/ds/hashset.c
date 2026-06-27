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
#include "vstd/ds/hashset.h"
#include "vstd/ds/iterator.h"
#include "vstd/ds/linked_list.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/utils.h"

typedef struct hashset_entry {
    linked_list_node node;
    size_t hash;
    max_align_t align;
    uint8_t data[];
} hashset_entry;

struct hashset {
    size_t size;
    size_t elem_size;
    size_t capacity;
    hash_common_bucket *buckets;
    hashset_elem_eq_fn elem_eq;
    allocator *allocator;
};

typedef struct hashset_iterator_state {
    const hashset *set;
    size_t bucket;
    linked_list_node *node;
} hashset_iterator_state;

_Static_assert(
    sizeof(hashset_iterator_state) <= ITERATOR_STATE_SIZE,
    "hashset_iterator_state must fit in iterator"
);

static void hashset_entry_destroy(linked_list_node *node, allocator *allocator) {
    hashset_entry *entry = CONTAINER_OF(node, hashset_entry, node);
    dealloc(allocator, entry);
}

static const void *hashset_entry_elem(const linked_list_node *node) {
    const hashset_entry *entry = CONTAINER_OF(node, hashset_entry, node);
    return entry->data;
}

static size_t hashset_entry_hash(const linked_list_node *node) {
    const hashset_entry *entry = CONTAINER_OF(node, hashset_entry, node);
    return entry->hash;
}

static hashset_entry *hashset_entry_get(const hashset *set, const void *elem) {
    size_t hash = fnv1a_hash(elem, set->elem_size);
    size_t bucket = hash % set->capacity;
    linked_list_node *node = hash_common_bucket_find(
        &set->buckets[bucket],
        elem,
        set->elem_size,
        hash,
        set->elem_eq,
        hashset_entry_elem,
        hashset_entry_hash
    );
    if (node != NULL) {
        return CONTAINER_OF(node, hashset_entry, node);
    }
    return NULL;
}

static const void *hashset_iterator_next(void *context) {
    ASSERT(context != NULL, "fatal: hashset_iterator_next invalid arguments");

    hashset_iterator_state *iterator = context;
    const hashset *set = iterator->set;

    while (iterator->node == NULL && iterator->bucket < set->capacity) {
        iterator->node = hash_common_bucket_head(&set->buckets[iterator->bucket]);
        iterator->bucket += 1;
    }

    if (iterator->node == NULL) {
        return NULL;
    }

    linked_list_node *node = iterator->node;
    hashset_entry *entry = CONTAINER_OF(node, hashset_entry, node);
    iterator->node = node->next;
    return entry->data;
}

status hashset_create(
    size_t elem_size,
    hashset_elem_eq_fn elem_eq,
    allocator *allocator,
    hashset **out
) {
    ASSERT(elem_size > 0, "fatal: hashset_create invalid arguments");
    ASSERT(out != NULL, "fatal: hashset_create invalid arguments");

    *out = NULL;

    size_t max_entry_size = 0;
    if (size_add_overflow(sizeof(hashset_entry), elem_size, &max_entry_size)) {
        return STATUS_OVERFLOW;
    }
    (void)max_entry_size;

    hashset *set = NULL;
    status st = alloc(allocator, sizeof(*set), (void **)&set);
    if (st != STATUS_OK) {
        return st;
    }

    set->allocator = allocator;
    st = hash_common_buckets_create(HASH_COMMON_DEFAULT_CAPACITY, allocator, &set->buckets);
    if (st != STATUS_OK) {
        dealloc(allocator, set);
        return st;
    }

    set->size = 0;
    set->elem_size = elem_size;
    set->capacity = HASH_COMMON_DEFAULT_CAPACITY;
    set->elem_eq = elem_eq;

    *out = set;
    return STATUS_OK;
}

status hashset_reserve(hashset *set, size_t size) {
    ASSERT(set != NULL, "fatal: hashset_reserve invalid arguments");

    size_t new_capacity = 0;
    RETURN_IF_ERROR(hash_common_capacity_for_size(size, &new_capacity));
    if (new_capacity <= set->capacity) {
        return STATUS_OK;
    }

    hash_common_bucket *new_buckets = NULL;
    status st = hash_common_buckets_rehash(
        set->buckets,
        set->capacity,
        new_capacity,
        set->allocator,
        hashset_entry_hash,
        &new_buckets
    );
    if (st != STATUS_OK) {
        return st;
    }

    set->buckets = new_buckets;
    set->capacity = new_capacity;
    return STATUS_OK;
}

status hashset_insert(hashset *set, const void *elem) {
    ASSERT(set != NULL, "fatal: hashset_insert invalid arguments");
    ASSERT(elem != NULL, "fatal: hashset_insert invalid arguments");

    allocator *allocator = set->allocator;
    size_t hash = fnv1a_hash(elem, set->elem_size);
    size_t bucket = hash % set->capacity;
    linked_list_node *node = hash_common_bucket_find(
        &set->buckets[bucket],
        elem,
        set->elem_size,
        hash,
        set->elem_eq,
        hashset_entry_elem,
        hashset_entry_hash
    );
    if (node != NULL) {
        return STATUS_OK;
    }

    if (hash_common_should_grow(set->size, set->capacity)) {
        size_t reserve_size = 0;
        if (size_add_overflow(set->size, 1, &reserve_size)) {
            return STATUS_OVERFLOW;
        }

        RETURN_IF_ERROR(hashset_reserve(set, reserve_size));
        bucket = hash % set->capacity;
    }

    hashset_entry *entry = NULL;
    size_t alloc_size = 0;
    if (size_add_overflow(sizeof(hashset_entry), set->elem_size, &alloc_size)) {
        return STATUS_OVERFLOW;
    }

    status st = alloc(allocator, alloc_size, (void **)&entry);
    if (st != STATUS_OK) {
        return st;
    }

    entry->hash = hash;
    memcpy(entry->data, elem, set->elem_size);

    hash_common_bucket_pushfront(&set->buckets[bucket], &entry->node);
    set->size += 1;
    return STATUS_OK;
}

bool hashset_contains(const hashset *set, const void *elem) {
    ASSERT(set != NULL, "fatal: hashset_contains invalid arguments");
    ASSERT(elem != NULL, "fatal: hashset_contains invalid arguments");

    return hashset_entry_get(set, elem) != NULL;
}

void *hashset_get(hashset *set, const void *elem) {
    ASSERT(set != NULL, "fatal: hashset_get invalid arguments");
    ASSERT(elem != NULL, "fatal: hashset_get invalid arguments");

    hashset_entry *entry = hashset_entry_get(set, elem);
    if (entry != NULL) {
        return entry->data;
    }
    return NULL;
}

const void *hashset_get_const(const hashset *set, const void *elem) {
    ASSERT(set != NULL, "fatal: hashset_get_const invalid arguments");
    ASSERT(elem != NULL, "fatal: hashset_get_const invalid arguments");

    hashset_entry *entry = hashset_entry_get(set, elem);
    if (entry != NULL) {
        return entry->data;
    }
    return NULL;
}

void hashset_remove(hashset *set, const void *elem) {
    ASSERT(set != NULL, "fatal: hashset_remove invalid arguments");
    ASSERT(elem != NULL, "fatal: hashset_remove invalid arguments");

    allocator *allocator = set->allocator;
    size_t hash = fnv1a_hash(elem, set->elem_size);
    size_t bucket = hash % set->capacity;
    linked_list_node *node = hash_common_bucket_remove(
        &set->buckets[bucket],
        elem,
        set->elem_size,
        hash,
        set->elem_eq,
        hashset_entry_elem,
        hashset_entry_hash
    );
    if (node != NULL) {
        hashset_entry *entry = CONTAINER_OF(node, hashset_entry, node);
        dealloc(allocator, entry);
        set->size -= 1;
    }
}

size_t hashset_size(const hashset *set) {
    ASSERT(set != NULL, "fatal: hashset_size invalid arguments");

    return set->size;
}

iterator hashset_get_iterator(const hashset *set) {
    ASSERT(set != NULL, "fatal: hashset_get_iterator invalid arguments");

    iterator iter = iterator_from_state(hashset_iterator_next);
    hashset_iterator_state *state = iterator_state(&iter);
    state->set = set;
    state->bucket = 0;
    state->node = NULL;
    iterator_set_size_hint(&iter, set->size);
    return iter;
}

void hashset_destroy(hashset *set) {
    ASSERT(set != NULL, "fatal: hashset_destroy invalid arguments");

    allocator *allocator = set->allocator;
    hash_common_buckets_destroy(set->buckets, set->capacity, allocator, hashset_entry_destroy);
    dealloc(allocator, set);
}
