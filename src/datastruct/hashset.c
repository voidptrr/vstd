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
#include "vstd/datastruct/hashset.h"
#include "vstd/datastruct/linked_list.h"
#include "vstd/memory/allocators/allocator.h"
#include "vstd/memory/utils.h"

typedef struct vs_hashset_entry {
    vs_linked_list_node node;
    max_align_t align;
    uint8_t data[];
} vs_hashset_entry;

struct vs_hashset {
    size_t size;
    size_t elem_size;
    size_t capacity;
    vs_linked_list **buckets;
    vs_hashset_elem_eq_fn elem_eq;
    vs_allocator *allocator;
};

static void vs_hashset_entry_destroy(vs_linked_list_node *node, vs_allocator *allocator) {
    vs_hashset_entry *entry = VS_CONTAINER_OF(node, vs_hashset_entry, node);
    vs_dealloc(allocator, entry);
}

static const void *vs_hashset_entry_elem(const vs_linked_list_node *node) {
    const vs_hashset_entry *entry = VS_CONTAINER_OF(node, vs_hashset_entry, node);
    return entry->data;
}

static vs_hashset_entry *vs_hashset_entry_get(const vs_hashset *set, const void *elem) {
    size_t bucket = vs_hash_common_bucket_index(elem, set->elem_size, set->capacity);
    vs_linked_list_node *node = vs_hash_common_bucket_find(
        set->buckets[bucket],
        elem,
        set->elem_size,
        set->elem_eq,
        vs_hashset_entry_elem
    );
    if (node != NULL) {
        return VS_CONTAINER_OF(node, vs_hashset_entry, node);
    }
    return NULL;
}

vs_hashset *vs_hashset_create(
    size_t elem_size,
    vs_hashset_elem_eq_fn elem_eq,
    vs_allocator *allocator
) {
    VSTD_ASSERT(elem_size > 0, "fatal: vs_hashset_create invalid arguments");

    vs_hashset *set = vs_malloc(allocator, sizeof(vs_hashset));
    set->allocator = allocator;
    set->buckets = vs_hash_common_buckets_create(VS_HASH_COMMON_DEFAULT_CAPACITY, allocator);

    set->size = 0;
    set->elem_size = elem_size;
    set->capacity = VS_HASH_COMMON_DEFAULT_CAPACITY;
    set->elem_eq = elem_eq;

    return set;
}

void vs_hashset_insert(vs_hashset *set, const void *elem) {
    VSTD_ASSERT(set != NULL, "fatal: vs_hashset_insert invalid arguments");
    VSTD_ASSERT(elem != NULL, "fatal: vs_hashset_insert invalid arguments");

    vs_allocator *allocator = set->allocator;
    size_t bucket = vs_hash_common_bucket_index(elem, set->elem_size, set->capacity);
    vs_linked_list_node *node = vs_hash_common_bucket_find(
        set->buckets[bucket],
        elem,
        set->elem_size,
        set->elem_eq,
        vs_hashset_entry_elem
    );
    if (node != NULL) {
        return;
    }

    if (vs_hash_common_should_grow(set->size, set->capacity)) {
        size_t new_capacity = set->capacity * 2;
        set->buckets = vs_hash_common_buckets_rehash(
            set->buckets,
            set->capacity,
            new_capacity,
            set->elem_size,
            allocator,
            vs_hashset_entry_elem
        );
        set->capacity = new_capacity;
        bucket = vs_hash_common_bucket_index(elem, set->elem_size, set->capacity);
    }

    vs_hashset_entry *entry = vs_malloc(allocator, sizeof(vs_hashset_entry) + set->elem_size);
    memcpy(entry->data, elem, set->elem_size);

    vs_linked_list_pushfront(set->buckets[bucket], &entry->node);
    set->size += 1;
}

bool vs_hashset_contains(const vs_hashset *set, const void *elem) {
    VSTD_ASSERT(set != NULL, "fatal: vs_hashset_contains invalid arguments");
    VSTD_ASSERT(elem != NULL, "fatal: vs_hashset_contains invalid arguments");

    return vs_hashset_entry_get(set, elem) != NULL;
}

void *vs_hashset_get(vs_hashset *set, const void *elem) {
    VSTD_ASSERT(set != NULL, "fatal: vs_hashset_get invalid arguments");
    VSTD_ASSERT(elem != NULL, "fatal: vs_hashset_get invalid arguments");

    vs_hashset_entry *entry = vs_hashset_entry_get(set, elem);
    if (entry != NULL) {
        return entry->data;
    }
    return NULL;
}

const void *vs_hashset_get_const(const vs_hashset *set, const void *elem) {
    VSTD_ASSERT(set != NULL, "fatal: vs_hashset_get_const invalid arguments");
    VSTD_ASSERT(elem != NULL, "fatal: vs_hashset_get_const invalid arguments");

    vs_hashset_entry *entry = vs_hashset_entry_get(set, elem);
    if (entry != NULL) {
        return entry->data;
    }
    return NULL;
}

void vs_hashset_remove(vs_hashset *set, const void *elem) {
    VSTD_ASSERT(set != NULL, "fatal: vs_hashset_remove invalid arguments");
    VSTD_ASSERT(elem != NULL, "fatal: vs_hashset_remove invalid arguments");

    vs_allocator *allocator = set->allocator;
    size_t bucket = vs_hash_common_bucket_index(elem, set->elem_size, set->capacity);
    vs_linked_list_node *node = vs_hash_common_bucket_remove(
        set->buckets[bucket],
        elem,
        set->elem_size,
        set->elem_eq,
        vs_hashset_entry_elem
    );
    if (node != NULL) {
        vs_hashset_entry *entry = VS_CONTAINER_OF(node, vs_hashset_entry, node);
        vs_dealloc(allocator, entry);
        set->size -= 1;
    }
}

size_t vs_hashset_size(const vs_hashset *set) {
    VSTD_ASSERT(set != NULL, "fatal: vs_hashset_size invalid arguments");

    return set->size;
}

void vs_hashset_destroy(vs_hashset *set) {
    VSTD_ASSERT(set != NULL, "fatal: vs_hashset_destroy invalid arguments");

    vs_allocator *allocator = set->allocator;
    vs_hash_common_buckets_destroy(
        set->buckets,
        set->capacity,
        allocator,
        vs_hashset_entry_destroy
    );
    vs_dealloc(allocator, set);
}
