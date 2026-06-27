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
#include <string.h>

#include "ds/hash_common.h"
#include "vstd/assert.h"
#include "vstd/ds/linked_list.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/utils.h"

static bool hash_common_value_eq(
    const void *lhs,
    const void *rhs,
    size_t size,
    hash_common_eq_fn value_eq
) {
    if (value_eq != NULL) {
        return value_eq(lhs, rhs);
    }
    return memcmp(lhs, rhs, size) == 0;
}

status hash_common_buckets_create(size_t capacity, allocator *allocator, hash_common_bucket **out) {
    ASSERT(capacity > 0, "fatal: hash_common_buckets_create invalid arguments");
    ASSERT(out != NULL, "fatal: hash_common_buckets_create invalid arguments");

    *out = NULL;

    size_t alloc_size = 0;
    if (size_mul_overflow(sizeof(hash_common_bucket), capacity, &alloc_size)) {
        return STATUS_OVERFLOW;
    }

    hash_common_bucket *buckets = NULL;
    status st = alloc(allocator, alloc_size, (void **)&buckets);
    if (st != STATUS_OK) {
        return st;
    }

    memset((void *)buckets, 0, alloc_size);

    *out = buckets;
    return STATUS_OK;
}

status hash_common_buckets_rehash(
    hash_common_bucket *buckets,
    size_t capacity,
    size_t new_capacity,
    allocator *allocator,
    hash_common_entry_hash_fn entry_hash,
    hash_common_bucket **out
) {
    ASSERT(buckets != NULL, "fatal: hash_common_buckets_rehash invalid arguments");
    ASSERT(capacity > 0, "fatal: hash_common_buckets_rehash invalid arguments");
    ASSERT(new_capacity > 0, "fatal: hash_common_buckets_rehash invalid arguments");
    ASSERT(entry_hash != NULL, "fatal: hash_common_buckets_rehash invalid arguments");
    ASSERT(out != NULL, "fatal: hash_common_buckets_rehash invalid arguments");

    *out = NULL;

    hash_common_bucket *new_buckets = NULL;
    status st = hash_common_buckets_create(new_capacity, allocator, &new_buckets);
    if (st != STATUS_OK) {
        return st;
    }

    for (size_t i = 0; i < capacity; i++) {
        linked_list_node *node = buckets[i].head;
        while (node != NULL) {
            linked_list_node *next = node->next;
            size_t bucket = entry_hash(node) % new_capacity;

            hash_common_bucket_pushfront(&new_buckets[bucket], node);
            node = next;
        }
    }

    dealloc(allocator, (void *)buckets);
    *out = new_buckets;
    return STATUS_OK;
}

linked_list_node *hash_common_bucket_find(
    const hash_common_bucket *bucket,
    const void *value,
    size_t value_size,
    size_t hash,
    hash_common_eq_fn value_eq,
    hash_common_entry_value_fn entry_value,
    hash_common_entry_hash_fn entry_hash
) {
    ASSERT(bucket != NULL, "fatal: hash_common_bucket_find invalid arguments");
    ASSERT(value != NULL, "fatal: hash_common_bucket_find invalid arguments");
    ASSERT(value_size > 0, "fatal: hash_common_bucket_find invalid arguments");
    ASSERT(entry_value != NULL, "fatal: hash_common_bucket_find invalid arguments");
    ASSERT(entry_hash != NULL, "fatal: hash_common_bucket_find invalid arguments");

    linked_list_node *node = bucket->head;
    while (node != NULL) {
        if (entry_hash(node) == hash
            && hash_common_value_eq(entry_value(node), value, value_size, value_eq)) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

linked_list_node *hash_common_bucket_remove(
    hash_common_bucket *bucket,
    const void *value,
    size_t value_size,
    size_t hash,
    hash_common_eq_fn value_eq,
    hash_common_entry_value_fn entry_value,
    hash_common_entry_hash_fn entry_hash
) {
    ASSERT(bucket != NULL, "fatal: hash_common_bucket_remove invalid arguments");
    ASSERT(value != NULL, "fatal: hash_common_bucket_remove invalid arguments");
    ASSERT(value_size > 0, "fatal: hash_common_bucket_remove invalid arguments");
    ASSERT(entry_value != NULL, "fatal: hash_common_bucket_remove invalid arguments");
    ASSERT(entry_hash != NULL, "fatal: hash_common_bucket_remove invalid arguments");

    linked_list_node *prev = NULL;
    linked_list_node *node = bucket->head;
    while (node != NULL) {
        if (entry_hash(node) == hash
            && hash_common_value_eq(entry_value(node), value, value_size, value_eq)) {
            if (prev == NULL) {
                bucket->head = node->next;
            } else {
                prev->next = node->next;
            }
            node->next = NULL;
            return node;
        }
        prev = node;
        node = node->next;
    }

    return NULL;
}

void hash_common_buckets_destroy(
    hash_common_bucket *buckets,
    size_t capacity,
    allocator *allocator,
    hash_common_entry_destroy_fn entry_destroy
) {
    ASSERT(buckets != NULL, "fatal: hash_common_buckets_destroy invalid arguments");
    ASSERT(entry_destroy != NULL, "fatal: hash_common_buckets_destroy invalid arguments");

    for (size_t i = 0; i < capacity; i++) {
        linked_list_node *node = buckets[i].head;
        while (node != NULL) {
            linked_list_node *next = node->next;
            entry_destroy(node, allocator);
            node = next;
        }
    }

    dealloc(allocator, (void *)buckets);
}

void hash_common_bucket_pushfront(hash_common_bucket *bucket, linked_list_node *node) {
    ASSERT(bucket != NULL, "fatal: hash_common_bucket_pushfront invalid arguments");
    ASSERT(node != NULL, "fatal: hash_common_bucket_pushfront invalid arguments");

    node->next = bucket->head;
    bucket->head = node;
}

linked_list_node *hash_common_bucket_head(const hash_common_bucket *bucket) {
    ASSERT(bucket != NULL, "fatal: hash_common_bucket_head invalid arguments");

    return bucket->head;
}
