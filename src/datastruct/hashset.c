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

#include "ckit/common/panic.h"
#include "ckit/datastruct/hashset.h"
#include "ckit/datastruct/linked_list.h"
#include "ckit/memory/allocators/allocator.h"
#include "ckit/utils.h"
#include "datastruct/hash_common.h"

typedef struct ck_hashset_entry {
    ck_linked_list_node node;
    max_align_t align;
    uint8_t data[];
} ck_hashset_entry;

struct ck_hashset {
    size_t size;
    size_t elem_size;
    size_t capacity;
    ck_linked_list **buckets;
    ck_hashset_elem_eq_fn elem_eq;
    ck_allocator *allocator;
};

static void ck_hashset_entry_destroy(ck_linked_list_node *node, ck_allocator *allocator) {
    ck_hashset_entry *entry = CK_CONTAINER_OF(node, ck_hashset_entry, node);
    ck_dealloc(allocator, entry);
}

static const void *ck_hashset_entry_elem(const ck_linked_list_node *node) {
    const ck_hashset_entry *entry = CK_CONTAINER_OF(node, ck_hashset_entry, node);
    return entry->data;
}

static ck_hashset_entry *ck_hashset_entry_get(const ck_hashset *set, const void *elem) {
    size_t bucket = ck_hash_common_bucket_index(elem, set->elem_size, set->capacity);
    ck_linked_list_node *node = ck_hash_common_bucket_find(
        set->buckets[bucket], elem, set->elem_size, set->elem_eq, ck_hashset_entry_elem);
    if (node != NULL) {
        return CK_CONTAINER_OF(node, ck_hashset_entry, node);
    }
    return NULL;
}

ck_hashset *ck_hashset_create(size_t elem_size, ck_hashset_elem_eq_fn elem_eq,
                              ck_allocator *allocator) {
    CK_ASSERT(elem_eq != NULL, "fatal: ck_hashset_create invalid arguments");
    CK_ASSERT(elem_size > 0, "fatal: ck_hashset_create invalid arguments");

    ck_hashset *set = ck_malloc(allocator, sizeof(ck_hashset));
    set->allocator = allocator;
    set->buckets = ck_hash_common_buckets_create(CK_HASH_COMMON_DEFAULT_CAPACITY, allocator);

    set->size = 0;
    set->elem_size = elem_size;
    set->capacity = CK_HASH_COMMON_DEFAULT_CAPACITY;
    set->elem_eq = elem_eq;

    return set;
}

void ck_hashset_insert(ck_hashset *set, const void *elem) {
    CK_ASSERT(set != NULL, "fatal: ck_hashset_insert invalid arguments");
    CK_ASSERT(elem != NULL, "fatal: ck_hashset_insert invalid arguments");

    size_t bucket = ck_hash_common_bucket_index(elem, set->elem_size, set->capacity);
    ck_linked_list_node *node = ck_hash_common_bucket_find(
        set->buckets[bucket], elem, set->elem_size, set->elem_eq, ck_hashset_entry_elem);
    if (node != NULL) {
        return;
    }

    if (ck_hash_common_should_grow(set->size, set->capacity)) {
        size_t new_capacity = set->capacity * 2;
        set->buckets =
            ck_hash_common_buckets_rehash(set->buckets, set->capacity, new_capacity, set->elem_size,
                                          set->allocator, ck_hashset_entry_elem);
        set->capacity = new_capacity;
        bucket = ck_hash_common_bucket_index(elem, set->elem_size, set->capacity);
    }

    ck_hashset_entry *entry = ck_malloc(set->allocator, sizeof(ck_hashset_entry) + set->elem_size);
    memcpy(entry->data, elem, set->elem_size);

    ck_linked_list_pushfront(set->buckets[bucket], &entry->node);
    set->size += 1;
}

bool ck_hashset_contains(const ck_hashset *set, const void *elem) {
    CK_ASSERT(set != NULL, "fatal: ck_hashset_contains invalid arguments");
    CK_ASSERT(elem != NULL, "fatal: ck_hashset_contains invalid arguments");

    return ck_hashset_entry_get(set, elem) != NULL;
}

void *ck_hashset_get(ck_hashset *set, const void *elem) {
    CK_ASSERT(set != NULL, "fatal: ck_hashset_get invalid arguments");
    CK_ASSERT(elem != NULL, "fatal: ck_hashset_get invalid arguments");

    ck_hashset_entry *entry = ck_hashset_entry_get(set, elem);
    if (entry != NULL) {
        return entry->data;
    }
    return NULL;
}

const void *ck_hashset_get_const(const ck_hashset *set, const void *elem) {
    CK_ASSERT(set != NULL, "fatal: ck_hashset_get_const invalid arguments");
    CK_ASSERT(elem != NULL, "fatal: ck_hashset_get_const invalid arguments");

    ck_hashset_entry *entry = ck_hashset_entry_get(set, elem);
    if (entry != NULL) {
        return entry->data;
    }
    return NULL;
}

void ck_hashset_remove(ck_hashset *set, const void *elem) {
    CK_ASSERT(set != NULL, "fatal: ck_hashset_remove invalid arguments");
    CK_ASSERT(elem != NULL, "fatal: ck_hashset_remove invalid arguments");

    size_t bucket = ck_hash_common_bucket_index(elem, set->elem_size, set->capacity);
    ck_linked_list_node *node = ck_hash_common_bucket_remove(
        set->buckets[bucket], elem, set->elem_size, set->elem_eq, ck_hashset_entry_elem);
    if (node != NULL) {
        ck_hashset_entry *entry = CK_CONTAINER_OF(node, ck_hashset_entry, node);
        ck_dealloc(set->allocator, entry);
        set->size -= 1;
    }
}

size_t ck_hashset_size(const ck_hashset *set) {
    CK_ASSERT(set != NULL, "fatal: ck_hashset_size invalid arguments");

    return set->size;
}

void ck_hashset_destroy(ck_hashset *set) {
    CK_ASSERT(set != NULL, "fatal: ck_hashset_destroy invalid arguments");

    ck_allocator *allocator = set->allocator;
    ck_hash_common_buckets_destroy(set->buckets, set->capacity, allocator,
                                   ck_hashset_entry_destroy);
    ck_dealloc(allocator, set);
}
