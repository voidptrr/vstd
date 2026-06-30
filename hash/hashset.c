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
#include "k4c/hash/hashset.h"

typedef struct k4c_hashset_entry {
    k4c_linked_list_node node;
    size_t hash;
    max_align_t align;
    uint8_t data[];
} k4c_hashset_entry;

struct k4c_hashset {
    size_t size;
    size_t elem_size;
    size_t capacity;
    k4c_hash_common_bucket *buckets;
    k4c_hashset_elem_eq_fn elem_eq;
    k4c_allocator k4c_allocator;
};

typedef struct k4c_hashset_iterator_state {
    const k4c_hashset *set;
    size_t bucket;
    k4c_linked_list_node *node;
} k4c_hashset_iterator_state;

_Static_assert(
    sizeof(k4c_hashset_iterator_state) <= K4C_ITERATOR_STATE_SIZE,
    "k4c_hashset_iterator_state must fit in k4c_iterator"
);

static void k4c_hashset_entry_destroy(k4c_linked_list_node *node, k4c_allocator *k4c_allocator) {
    k4c_hashset_entry *entry = K4C_CONTAINER_OF(node, k4c_hashset_entry, node);
    k4c_dealloc(k4c_allocator, entry);
}

static const void *k4c_hashset_entry_elem(const k4c_linked_list_node *node) {
    const k4c_hashset_entry *entry = K4C_CONTAINER_OF(node, k4c_hashset_entry, node);
    return entry->data;
}

static size_t k4c_hashset_entry_hash(const k4c_linked_list_node *node) {
    const k4c_hashset_entry *entry = K4C_CONTAINER_OF(node, k4c_hashset_entry, node);
    return entry->hash;
}

static k4c_hashset_entry *k4c_hashset_entry_get(const k4c_hashset *set, const void *elem) {
    size_t hash = k4c_fnv1a_hash(elem, set->elem_size);
    size_t bucket = hash % set->capacity;
    k4c_linked_list_node *node = k4c_hash_common_bucket_find(
        &set->buckets[bucket],
        elem,
        set->elem_size,
        hash,
        set->elem_eq,
        k4c_hashset_entry_elem,
        k4c_hashset_entry_hash
    );
    if (node != NULL) {
        return K4C_CONTAINER_OF(node, k4c_hashset_entry, node);
    }
    return NULL;
}

static const void *k4c_hashset_iterator_next(void *context) {
    K4C_ASSERT(context != NULL, "fatal: k4c_hashset_iterator_next invalid arguments");

    k4c_hashset_iterator_state *k4c_iterator = context;
    const k4c_hashset *set = k4c_iterator->set;

    while (k4c_iterator->node == NULL && k4c_iterator->bucket < set->capacity) {
        k4c_iterator->node = k4c_hash_common_bucket_head(&set->buckets[k4c_iterator->bucket]);
        k4c_iterator->bucket += 1;
    }

    if (k4c_iterator->node == NULL) {
        return NULL;
    }

    k4c_linked_list_node *node = k4c_iterator->node;
    k4c_hashset_entry *entry = K4C_CONTAINER_OF(node, k4c_hashset_entry, node);
    k4c_iterator->node = node->next;
    return entry->data;
}

k4c_status k4c_hashset_create(
    size_t elem_size,
    k4c_hashset_elem_eq_fn elem_eq,
    k4c_allocator *k4c_allocator,
    k4c_hashset **out
) {
    K4C_ASSERT(elem_size > 0, "fatal: k4c_hashset_create invalid arguments");
    K4C_ASSERT(out != NULL, "fatal: k4c_hashset_create invalid arguments");

    *out = NULL;

    size_t max_entry_size = 0;
    if (k4c_size_add_overflow(sizeof(k4c_hashset_entry), elem_size, &max_entry_size)) {
        return K4C_STATUS_OVERFLOW;
    }
    (void)max_entry_size;

    k4c_hashset *set = NULL;
    k4c_status st = k4c_alloc(k4c_allocator, sizeof(*set), (void **)&set);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    set->k4c_allocator = k4c_allocator_copy(k4c_allocator);
    st = k4c_hash_common_buckets_create(
        K4C_HASH_COMMON_DEFAULT_CAPACITY,
        &set->k4c_allocator,
        &set->buckets
    );
    if (st != K4C_STATUS_OK) {
        k4c_dealloc(&set->k4c_allocator, set);
        return st;
    }

    set->size = 0;
    set->elem_size = elem_size;
    set->capacity = K4C_HASH_COMMON_DEFAULT_CAPACITY;
    set->elem_eq = elem_eq;

    *out = set;
    return K4C_STATUS_OK;
}

k4c_status k4c_hashset_reserve(k4c_hashset *set, size_t size) {
    K4C_ASSERT(set != NULL, "fatal: k4c_hashset_reserve invalid arguments");

    size_t new_capacity = 0;
    K4C_RETURN_IF_ERROR(k4c_hash_common_capacity_for_size(size, &new_capacity));
    if (new_capacity <= set->capacity) {
        return K4C_STATUS_OK;
    }

    k4c_hash_common_bucket *new_buckets = NULL;
    k4c_status st = k4c_hash_common_buckets_rehash(
        set->buckets,
        set->capacity,
        new_capacity,
        &set->k4c_allocator,
        k4c_hashset_entry_hash,
        &new_buckets
    );
    if (st != K4C_STATUS_OK) {
        return st;
    }

    set->buckets = new_buckets;
    set->capacity = new_capacity;
    return K4C_STATUS_OK;
}

k4c_status k4c_hashset_insert(k4c_hashset *set, const void *elem) {
    K4C_ASSERT(set != NULL, "fatal: k4c_hashset_insert invalid arguments");
    K4C_ASSERT(elem != NULL, "fatal: k4c_hashset_insert invalid arguments");

    k4c_allocator *k4c_allocator = &set->k4c_allocator;
    size_t hash = k4c_fnv1a_hash(elem, set->elem_size);
    size_t bucket = hash % set->capacity;
    k4c_linked_list_node *node = k4c_hash_common_bucket_find(
        &set->buckets[bucket],
        elem,
        set->elem_size,
        hash,
        set->elem_eq,
        k4c_hashset_entry_elem,
        k4c_hashset_entry_hash
    );
    if (node != NULL) {
        return K4C_STATUS_OK;
    }

    if (k4c_hash_common_should_grow(set->size, set->capacity)) {
        size_t reserve_size = 0;
        if (k4c_size_add_overflow(set->size, 1, &reserve_size)) {
            return K4C_STATUS_OVERFLOW;
        }

        K4C_RETURN_IF_ERROR(k4c_hashset_reserve(set, reserve_size));
        bucket = hash % set->capacity;
    }

    k4c_hashset_entry *entry = NULL;
    size_t alloc_size = 0;
    if (k4c_size_add_overflow(sizeof(k4c_hashset_entry), set->elem_size, &alloc_size)) {
        return K4C_STATUS_OVERFLOW;
    }

    k4c_status st = k4c_alloc(k4c_allocator, alloc_size, (void **)&entry);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    entry->hash = hash;
    memcpy(entry->data, elem, set->elem_size);

    k4c_hash_common_bucket_pushfront(&set->buckets[bucket], &entry->node);
    set->size += 1;
    return K4C_STATUS_OK;
}

bool k4c_hashset_contains(const k4c_hashset *set, const void *elem) {
    K4C_ASSERT(set != NULL, "fatal: k4c_hashset_contains invalid arguments");
    K4C_ASSERT(elem != NULL, "fatal: k4c_hashset_contains invalid arguments");

    return k4c_hashset_entry_get(set, elem) != NULL;
}

void *k4c_hashset_get(const k4c_hashset *set, const void *elem) {
    K4C_ASSERT(set != NULL, "fatal: k4c_hashset_get invalid arguments");
    K4C_ASSERT(elem != NULL, "fatal: k4c_hashset_get invalid arguments");

    k4c_hashset_entry *entry = k4c_hashset_entry_get(set, elem);
    if (entry != NULL) {
        return entry->data;
    }
    return NULL;
}

void k4c_hashset_remove(k4c_hashset *set, const void *elem) {
    K4C_ASSERT(set != NULL, "fatal: k4c_hashset_remove invalid arguments");
    K4C_ASSERT(elem != NULL, "fatal: k4c_hashset_remove invalid arguments");

    k4c_allocator *k4c_allocator = &set->k4c_allocator;
    size_t hash = k4c_fnv1a_hash(elem, set->elem_size);
    size_t bucket = hash % set->capacity;
    k4c_linked_list_node *node = k4c_hash_common_bucket_remove(
        &set->buckets[bucket],
        elem,
        set->elem_size,
        hash,
        set->elem_eq,
        k4c_hashset_entry_elem,
        k4c_hashset_entry_hash
    );
    if (node != NULL) {
        k4c_hashset_entry *entry = K4C_CONTAINER_OF(node, k4c_hashset_entry, node);
        k4c_dealloc(k4c_allocator, entry);
        set->size -= 1;
    }
}

size_t k4c_hashset_size(const k4c_hashset *set) {
    K4C_ASSERT(set != NULL, "fatal: k4c_hashset_size invalid arguments");

    return set->size;
}

k4c_iterator k4c_hashset_get_iterator(const k4c_hashset *set) {
    K4C_ASSERT(set != NULL, "fatal: k4c_hashset_get_iterator invalid arguments");

    k4c_iterator iter = k4c_iterator_from_state(k4c_hashset_iterator_next);
    k4c_hashset_iterator_state *state = k4c_iterator_state(&iter);
    state->set = set;
    state->bucket = 0;
    state->node = NULL;
    k4c_iterator_set_size_hint(&iter, set->size);
    return iter;
}

void k4c_hashset_destroy(k4c_hashset *set) {
    K4C_ASSERT(set != NULL, "fatal: k4c_hashset_destroy invalid arguments");

    k4c_allocator *k4c_allocator = &set->k4c_allocator;
    k4c_hash_common_buckets_destroy(
        set->buckets,
        set->capacity,
        k4c_allocator,
        k4c_hashset_entry_destroy
    );
    k4c_dealloc(k4c_allocator, set);
}
