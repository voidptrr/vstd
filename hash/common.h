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

#ifndef K4C_HASH_COMMON_H
#define K4C_HASH_COMMON_H

#include <stdbool.h>
#include <stddef.h>

#include "allocators/size.h"
#include "k4c/allocators/allocator.h"
#include "k4c/assert.h"
#include "k4c/datastruct/linked_list.h"
#include "k4c/error.h"

#define K4C_HASH_COMMON_DEFAULT_CAPACITY 16
#define K4C_HASH_COMMON_MAX_LOAD_NUMERATOR 3
#define K4C_HASH_COMMON_MAX_LOAD_DENOMINATOR 4

typedef void (*k4c_hash_common_entry_destroy_fn)(
    k4c_linked_list_node *node,
    k4c_allocator *k4c_allocator
);
typedef const void *(*k4c_hash_common_entry_value_fn)(const k4c_linked_list_node *node);
typedef size_t (*k4c_hash_common_entry_hash_fn)(const k4c_linked_list_node *node);
typedef bool (*k4c_hash_common_eq_fn)(const void *lhs, const void *rhs);

typedef struct k4c_hash_common_bucket {
    k4c_linked_list_node *head;
} k4c_hash_common_bucket;

/* Return true when inserting one more item would exceed the load limit. */
static inline bool k4c_hash_common_should_grow(size_t size, size_t capacity) {
    size_t next_size = 0;
    size_t scaled_size = 0;
    size_t scaled_capacity = 0;
    if (k4c_size_add_overflow(size, 1, &next_size)
        || k4c_size_mul_overflow(next_size, K4C_HASH_COMMON_MAX_LOAD_DENOMINATOR, &scaled_size)) {
        return true;
    }

    if (k4c_size_mul_overflow(capacity, K4C_HASH_COMMON_MAX_LOAD_NUMERATOR, &scaled_capacity)) {
        return false;
    }

    return scaled_size > scaled_capacity;
}

/* Return a power-of-two capacity that can hold size items at the max load. */
static inline k4c_status k4c_hash_common_capacity_for_size(size_t size, size_t *out) {
    K4C_ASSERT(out != NULL, "fatal: k4c_hash_common_capacity_for_size invalid arguments");

    size_t capacity = K4C_HASH_COMMON_DEFAULT_CAPACITY;
    size_t scaled_size = 0;
    if (k4c_size_mul_overflow(size, K4C_HASH_COMMON_MAX_LOAD_DENOMINATOR, &scaled_size)) {
        return K4C_STATUS_OVERFLOW;
    }

    while (1) {
        size_t scaled_capacity = 0;
        if (k4c_size_mul_overflow(capacity, K4C_HASH_COMMON_MAX_LOAD_NUMERATOR, &scaled_capacity)) {
            return K4C_STATUS_OVERFLOW;
        }
        if (scaled_size <= scaled_capacity) {
            break;
        }
        if (k4c_size_mul_overflow(capacity, 2, &capacity)) {
            return K4C_STATUS_OVERFLOW;
        }
    }

    *out = capacity;
    return K4C_STATUS_OK;
}

/* Allocate and initialize an array of empty bucket lists. */
k4c_status k4c_hash_common_buckets_create(
    size_t capacity,
    k4c_allocator *k4c_allocator,
    k4c_hash_common_bucket **out
);

/*
 * Move all nodes into a newly allocated bucket array sized to new_capacity.
 * The old bucket array is destroyed, but entries are preserved and relinked
 * into the new buckets.
 */
k4c_status k4c_hash_common_buckets_rehash(
    k4c_hash_common_bucket *buckets,
    size_t capacity,
    size_t new_capacity,
    k4c_allocator *k4c_allocator,
    k4c_hash_common_entry_hash_fn entry_hash,
    k4c_hash_common_bucket **out
);

/* Return the first node whose extracted value equals value, or NULL. */
k4c_linked_list_node *k4c_hash_common_bucket_find(
    const k4c_hash_common_bucket *bucket,
    const void *value,
    size_t value_size,
    size_t hash,
    k4c_hash_common_eq_fn value_eq,
    k4c_hash_common_entry_value_fn entry_value,
    k4c_hash_common_entry_hash_fn entry_hash
);

/*
 * Unlink and return the first matching node from bucket, or NULL when no match
 * is found. The returned node remains owned by the caller.
 */
k4c_linked_list_node *k4c_hash_common_bucket_remove(
    k4c_hash_common_bucket *bucket,
    const void *value,
    size_t value_size,
    size_t hash,
    k4c_hash_common_eq_fn value_eq,
    k4c_hash_common_entry_value_fn entry_value,
    k4c_hash_common_entry_hash_fn entry_hash
);

/* Destroy all entries, bucket lists, and the bucket array. */
void k4c_hash_common_buckets_destroy(
    k4c_hash_common_bucket *buckets,
    size_t capacity,
    k4c_allocator *k4c_allocator,
    k4c_hash_common_entry_destroy_fn entry_destroy
);

/* Link node at the front of bucket. */
void k4c_hash_common_bucket_pushfront(k4c_hash_common_bucket *bucket, k4c_linked_list_node *node);

/* Return the first node in bucket, or NULL when empty. */
k4c_linked_list_node *k4c_hash_common_bucket_head(const k4c_hash_common_bucket *bucket);

#endif
