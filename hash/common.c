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

#include "allocators/size.h"
#include "hash/common.h"
#include "k4c/allocators/allocator.h"
#include "k4c/assert.h"
#include "k4c/datastruct/linked_list.h"
#include "k4c/error.h"

static bool k4c_hash_common_value_eq(
    const void *lhs,
    const void *rhs,
    size_t size,
    k4c_hash_common_eq_fn value_eq
) {
    if (value_eq != NULL) {
        return value_eq(lhs, rhs);
    }
    return memcmp(lhs, rhs, size) == 0;
}

k4c_status k4c_hash_common_buckets_create(
    size_t capacity,
    k4c_allocator *k4c_allocator,
    k4c_hash_common_bucket **out
) {
    K4C_ASSERT(capacity > 0, "fatal: k4c_hash_common_buckets_create invalid arguments");
    K4C_ASSERT(out != NULL, "fatal: k4c_hash_common_buckets_create invalid arguments");

    *out = NULL;

    size_t alloc_size = 0;
    if (k4c_size_mul_overflow(sizeof(k4c_hash_common_bucket), capacity, &alloc_size)) {
        return K4C_STATUS_OVERFLOW;
    }

    k4c_hash_common_bucket *buckets = NULL;
    k4c_status st = k4c_alloc(k4c_allocator, alloc_size, (void **)&buckets);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    memset((void *)buckets, 0, alloc_size);

    *out = buckets;
    return K4C_STATUS_OK;
}

k4c_status k4c_hash_common_buckets_rehash(
    k4c_hash_common_bucket *buckets,
    size_t capacity,
    size_t new_capacity,
    k4c_allocator *k4c_allocator,
    k4c_hash_common_entry_hash_fn entry_hash,
    k4c_hash_common_bucket **out
) {
    K4C_ASSERT(buckets != NULL, "fatal: k4c_hash_common_buckets_rehash invalid arguments");
    K4C_ASSERT(capacity > 0, "fatal: k4c_hash_common_buckets_rehash invalid arguments");
    K4C_ASSERT(new_capacity > 0, "fatal: k4c_hash_common_buckets_rehash invalid arguments");
    K4C_ASSERT(entry_hash != NULL, "fatal: k4c_hash_common_buckets_rehash invalid arguments");
    K4C_ASSERT(out != NULL, "fatal: k4c_hash_common_buckets_rehash invalid arguments");

    *out = NULL;

    k4c_hash_common_bucket *new_buckets = NULL;
    k4c_status st = k4c_hash_common_buckets_create(new_capacity, k4c_allocator, &new_buckets);
    if (st != K4C_STATUS_OK) {
        return st;
    }

    for (size_t i = 0; i < capacity; i++) {
        k4c_linked_list_node *node = buckets[i].head;
        while (node != NULL) {
            k4c_linked_list_node *next = node->next;
            size_t bucket = entry_hash(node) % new_capacity;

            k4c_hash_common_bucket_pushfront(&new_buckets[bucket], node);
            node = next;
        }
    }

    k4c_dealloc(k4c_allocator, (void *)buckets);
    *out = new_buckets;
    return K4C_STATUS_OK;
}

k4c_linked_list_node *k4c_hash_common_bucket_find(
    const k4c_hash_common_bucket *bucket,
    const void *value,
    size_t value_size,
    size_t hash,
    k4c_hash_common_eq_fn value_eq,
    k4c_hash_common_entry_value_fn entry_value,
    k4c_hash_common_entry_hash_fn entry_hash
) {
    K4C_ASSERT(bucket != NULL, "fatal: k4c_hash_common_bucket_find invalid arguments");
    K4C_ASSERT(value != NULL, "fatal: k4c_hash_common_bucket_find invalid arguments");
    K4C_ASSERT(value_size > 0, "fatal: k4c_hash_common_bucket_find invalid arguments");
    K4C_ASSERT(entry_value != NULL, "fatal: k4c_hash_common_bucket_find invalid arguments");
    K4C_ASSERT(entry_hash != NULL, "fatal: k4c_hash_common_bucket_find invalid arguments");

    k4c_linked_list_node *node = bucket->head;
    while (node != NULL) {
        if (entry_hash(node) == hash
            && k4c_hash_common_value_eq(entry_value(node), value, value_size, value_eq)) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

k4c_linked_list_node *k4c_hash_common_bucket_remove(
    k4c_hash_common_bucket *bucket,
    const void *value,
    size_t value_size,
    size_t hash,
    k4c_hash_common_eq_fn value_eq,
    k4c_hash_common_entry_value_fn entry_value,
    k4c_hash_common_entry_hash_fn entry_hash
) {
    K4C_ASSERT(bucket != NULL, "fatal: k4c_hash_common_bucket_remove invalid arguments");
    K4C_ASSERT(value != NULL, "fatal: k4c_hash_common_bucket_remove invalid arguments");
    K4C_ASSERT(value_size > 0, "fatal: k4c_hash_common_bucket_remove invalid arguments");
    K4C_ASSERT(entry_value != NULL, "fatal: k4c_hash_common_bucket_remove invalid arguments");
    K4C_ASSERT(entry_hash != NULL, "fatal: k4c_hash_common_bucket_remove invalid arguments");

    k4c_linked_list_node *prev = NULL;
    k4c_linked_list_node *node = bucket->head;
    while (node != NULL) {
        if (entry_hash(node) == hash
            && k4c_hash_common_value_eq(entry_value(node), value, value_size, value_eq)) {
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

void k4c_hash_common_buckets_destroy(
    k4c_hash_common_bucket *buckets,
    size_t capacity,
    k4c_allocator *k4c_allocator,
    k4c_hash_common_entry_destroy_fn entry_destroy
) {
    K4C_ASSERT(buckets != NULL, "fatal: k4c_hash_common_buckets_destroy invalid arguments");
    K4C_ASSERT(entry_destroy != NULL, "fatal: k4c_hash_common_buckets_destroy invalid arguments");

    for (size_t i = 0; i < capacity; i++) {
        k4c_linked_list_node *node = buckets[i].head;
        while (node != NULL) {
            k4c_linked_list_node *next = node->next;
            entry_destroy(node, k4c_allocator);
            node = next;
        }
    }

    k4c_dealloc(k4c_allocator, (void *)buckets);
}

void k4c_hash_common_bucket_pushfront(k4c_hash_common_bucket *bucket, k4c_linked_list_node *node) {
    K4C_ASSERT(bucket != NULL, "fatal: k4c_hash_common_bucket_pushfront invalid arguments");
    K4C_ASSERT(node != NULL, "fatal: k4c_hash_common_bucket_pushfront invalid arguments");

    node->next = bucket->head;
    bucket->head = node;
}

k4c_linked_list_node *k4c_hash_common_bucket_head(const k4c_hash_common_bucket *bucket) {
    K4C_ASSERT(bucket != NULL, "fatal: k4c_hash_common_bucket_head invalid arguments");

    return bucket->head;
}
