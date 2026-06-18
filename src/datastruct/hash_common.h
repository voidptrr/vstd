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

#ifndef VSTD_HASH_COMMON_H
#define VSTD_HASH_COMMON_H

#include <stdbool.h>
#include <stddef.h>

#include "hash.h"
#include "vstd/datastruct/linked_list.h"
#include "vstd/memory/allocators/allocator.h"

#define VS_HASH_COMMON_DEFAULT_CAPACITY 16
#define VS_HASH_COMMON_MAX_LOAD 0.75

typedef void (*vs_hash_common_entry_destroy_fn)(vs_linked_list_node *node, vs_allocator *allocator);
typedef const void *(*vs_hash_common_entry_value_fn)(const vs_linked_list_node *node);
typedef bool (*vs_hash_common_eq_fn)(const void *lhs, const void *rhs);

/* Allocate and initialize an array of empty bucket lists. */
vs_linked_list **vs_hash_common_buckets_create(size_t capacity, vs_allocator *allocator);

/*
 * Move all nodes into a newly allocated bucket array sized to new_capacity.
 * The old bucket list wrappers and bucket array are destroyed, but entries are
 * preserved and relinked into the new buckets.
 */
vs_linked_list **vs_hash_common_buckets_rehash(
    vs_linked_list **buckets,
    size_t capacity,
    size_t new_capacity,
    size_t value_size,
    vs_allocator *allocator,
    vs_hash_common_entry_value_fn entry_value
);

/* Return the first node whose extracted value equals value, or NULL. */
vs_linked_list_node *vs_hash_common_bucket_find(
    vs_linked_list *bucket,
    const void *value,
    size_t value_size,
    vs_hash_common_eq_fn value_eq,
    vs_hash_common_entry_value_fn entry_value
);

/*
 * Unlink and return the first matching node from bucket, or NULL when no match
 * is found. The returned node remains owned by the caller.
 */
vs_linked_list_node *vs_hash_common_bucket_remove(
    vs_linked_list *bucket,
    const void *value,
    size_t value_size,
    vs_hash_common_eq_fn value_eq,
    vs_hash_common_entry_value_fn entry_value
);

/* Destroy all entries, bucket lists, and the bucket array. */
void vs_hash_common_buckets_destroy(
    vs_linked_list **buckets,
    size_t capacity,
    vs_allocator *allocator,
    vs_hash_common_entry_destroy_fn entry_destroy
);

/* Hash value and map it into the bucket range [0, capacity). */
static inline size_t vs_hash_common_bucket_index(
    const void *value,
    size_t value_size,
    size_t capacity
) {
    return vs_fnv1a_hash(value, value_size) % capacity;
}

/* Return true when inserting one more item would exceed the load limit. */
static inline bool vs_hash_common_should_grow(size_t size, size_t capacity) {
    return ((double)(size + 1) / (double)capacity) > VS_HASH_COMMON_MAX_LOAD;
}

#endif
