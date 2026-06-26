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

#include "vstd/datastruct/linked_list.h"
#include "vstd/memory/allocator.h"

#define VS_HASH_COMMON_DEFAULT_CAPACITY 16
#define VS_HASH_COMMON_MAX_LOAD_NUMERATOR 3
#define VS_HASH_COMMON_MAX_LOAD_DENOMINATOR 4

typedef void (*vs_hash_common_entry_destroy_fn)(vs_linked_list_node *node, vs_allocator *allocator);
typedef const void *(*vs_hash_common_entry_value_fn)(const vs_linked_list_node *node);
typedef size_t (*vs_hash_common_entry_hash_fn)(const vs_linked_list_node *node);
typedef bool (*vs_hash_common_eq_fn)(const void *lhs, const void *rhs);

typedef struct vs_hash_common_bucket {
    vs_linked_list_node *head;
} vs_hash_common_bucket;

/* Hash value and map it into the bucket range [0, capacity). */
static inline size_t vs_hash_common_bucket_index(size_t hash, size_t capacity) {
    return hash % capacity;
}

/* Return true when inserting one more item would exceed the load limit. */
static inline bool vs_hash_common_should_grow(size_t size, size_t capacity) {
    return ((size + 1) * VS_HASH_COMMON_MAX_LOAD_DENOMINATOR)
           > (capacity * VS_HASH_COMMON_MAX_LOAD_NUMERATOR);
}

/* Return a power-of-two capacity that can hold size items at the max load. */
static inline size_t vs_hash_common_capacity_for_size(size_t size) {
    size_t capacity = VS_HASH_COMMON_DEFAULT_CAPACITY;
    while ((size * VS_HASH_COMMON_MAX_LOAD_DENOMINATOR)
           > (capacity * VS_HASH_COMMON_MAX_LOAD_NUMERATOR)) {
        capacity *= 2;
    }
    return capacity;
}

/* Allocate and initialize an array of empty bucket lists. */
vs_hash_common_bucket *vs_hash_common_buckets_create(size_t capacity, vs_allocator *allocator);

/*
 * Move all nodes into a newly allocated bucket array sized to new_capacity.
 * The old bucket array is destroyed, but entries are preserved and relinked
 * into the new buckets.
 */
vs_hash_common_bucket *vs_hash_common_buckets_rehash(
    vs_hash_common_bucket *buckets,
    size_t capacity,
    size_t new_capacity,
    vs_allocator *allocator,
    vs_hash_common_entry_hash_fn entry_hash
);

/* Return the first node whose extracted value equals value, or NULL. */
vs_linked_list_node *vs_hash_common_bucket_find(
    const vs_hash_common_bucket *bucket,
    const void *value,
    size_t value_size,
    size_t hash,
    vs_hash_common_eq_fn value_eq,
    vs_hash_common_entry_value_fn entry_value,
    vs_hash_common_entry_hash_fn entry_hash
);

/*
 * Unlink and return the first matching node from bucket, or NULL when no match
 * is found. The returned node remains owned by the caller.
 */
vs_linked_list_node *vs_hash_common_bucket_remove(
    vs_hash_common_bucket *bucket,
    const void *value,
    size_t value_size,
    size_t hash,
    vs_hash_common_eq_fn value_eq,
    vs_hash_common_entry_value_fn entry_value,
    vs_hash_common_entry_hash_fn entry_hash
);

/* Destroy all entries, bucket lists, and the bucket array. */
void vs_hash_common_buckets_destroy(
    vs_hash_common_bucket *buckets,
    size_t capacity,
    vs_allocator *allocator,
    vs_hash_common_entry_destroy_fn entry_destroy
);

/* Link node at the front of bucket. */
void vs_hash_common_bucket_pushfront(vs_hash_common_bucket *bucket, vs_linked_list_node *node);

/* Return the first node in bucket, or NULL when empty. */
vs_linked_list_node *vs_hash_common_bucket_head(const vs_hash_common_bucket *bucket);

#endif
