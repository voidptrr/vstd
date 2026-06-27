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

#include "datastruct/hash_common.h"
#include "vstd/assert.h"
#include "vstd/datastruct/linked_list.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"
#include "vstd/memory/utils.h"

static bool vs_hash_common_value_eq(
    const void *lhs,
    const void *rhs,
    size_t size,
    vs_hash_common_eq_fn value_eq
) {
    if (value_eq != NULL) {
        return value_eq(lhs, rhs);
    }
    return memcmp(lhs, rhs, size) == 0;
}

vs_status vs_hash_common_buckets_create(
    size_t capacity,
    vs_allocator *allocator,
    vs_hash_common_bucket **out
) {
    VSTD_ASSERT(capacity > 0, "fatal: vs_hash_common_buckets_create invalid arguments");
    VSTD_ASSERT(out != NULL, "fatal: vs_hash_common_buckets_create invalid arguments");

    *out = NULL;

    size_t alloc_size = 0;
    if (vs_size_mul_overflow(sizeof(vs_hash_common_bucket), capacity, &alloc_size)) {
        return VS_STATUS_OVERFLOW;
    }

    vs_hash_common_bucket *buckets = NULL;
    vs_status status = vs_alloc(allocator, alloc_size, (void **)&buckets);
    if (status != VS_STATUS_OK) {
        return status;
    }

    memset((void *)buckets, 0, alloc_size);

    *out = buckets;
    return VS_STATUS_OK;
}

vs_status vs_hash_common_buckets_rehash(
    vs_hash_common_bucket *buckets,
    size_t capacity,
    size_t new_capacity,
    vs_allocator *allocator,
    vs_hash_common_entry_hash_fn entry_hash,
    vs_hash_common_bucket **out
) {
    VSTD_ASSERT(buckets != NULL, "fatal: vs_hash_common_buckets_rehash invalid arguments");
    VSTD_ASSERT(capacity > 0, "fatal: vs_hash_common_buckets_rehash invalid arguments");
    VSTD_ASSERT(new_capacity > 0, "fatal: vs_hash_common_buckets_rehash invalid arguments");
    VSTD_ASSERT(entry_hash != NULL, "fatal: vs_hash_common_buckets_rehash invalid arguments");
    VSTD_ASSERT(out != NULL, "fatal: vs_hash_common_buckets_rehash invalid arguments");

    *out = NULL;

    vs_hash_common_bucket *new_buckets = NULL;
    vs_status status = vs_hash_common_buckets_create(new_capacity, allocator, &new_buckets);
    if (status != VS_STATUS_OK) {
        return status;
    }

    for (size_t i = 0; i < capacity; i++) {
        vs_linked_list_node *node = buckets[i].head;
        while (node != NULL) {
            vs_linked_list_node *next = node->next;
            size_t bucket = vs_hash_common_bucket_index(entry_hash(node), new_capacity);

            vs_hash_common_bucket_pushfront(&new_buckets[bucket], node);
            node = next;
        }
    }

    vs_dealloc(allocator, (void *)buckets);
    *out = new_buckets;
    return VS_STATUS_OK;
}

vs_linked_list_node *vs_hash_common_bucket_find(
    const vs_hash_common_bucket *bucket,
    const void *value,
    size_t value_size,
    size_t hash,
    vs_hash_common_eq_fn value_eq,
    vs_hash_common_entry_value_fn entry_value,
    vs_hash_common_entry_hash_fn entry_hash
) {
    VSTD_ASSERT(bucket != NULL, "fatal: vs_hash_common_bucket_find invalid arguments");
    VSTD_ASSERT(value != NULL, "fatal: vs_hash_common_bucket_find invalid arguments");
    VSTD_ASSERT(value_size > 0, "fatal: vs_hash_common_bucket_find invalid arguments");
    VSTD_ASSERT(entry_value != NULL, "fatal: vs_hash_common_bucket_find invalid arguments");
    VSTD_ASSERT(entry_hash != NULL, "fatal: vs_hash_common_bucket_find invalid arguments");

    vs_linked_list_node *node = bucket->head;
    while (node != NULL) {
        if (entry_hash(node) == hash
            && vs_hash_common_value_eq(entry_value(node), value, value_size, value_eq)) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

vs_linked_list_node *vs_hash_common_bucket_remove(
    vs_hash_common_bucket *bucket,
    const void *value,
    size_t value_size,
    size_t hash,
    vs_hash_common_eq_fn value_eq,
    vs_hash_common_entry_value_fn entry_value,
    vs_hash_common_entry_hash_fn entry_hash
) {
    VSTD_ASSERT(bucket != NULL, "fatal: vs_hash_common_bucket_remove invalid arguments");
    VSTD_ASSERT(value != NULL, "fatal: vs_hash_common_bucket_remove invalid arguments");
    VSTD_ASSERT(value_size > 0, "fatal: vs_hash_common_bucket_remove invalid arguments");
    VSTD_ASSERT(entry_value != NULL, "fatal: vs_hash_common_bucket_remove invalid arguments");
    VSTD_ASSERT(entry_hash != NULL, "fatal: vs_hash_common_bucket_remove invalid arguments");

    vs_linked_list_node *prev = NULL;
    vs_linked_list_node *node = bucket->head;
    while (node != NULL) {
        if (entry_hash(node) == hash
            && vs_hash_common_value_eq(entry_value(node), value, value_size, value_eq)) {
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

void vs_hash_common_buckets_destroy(
    vs_hash_common_bucket *buckets,
    size_t capacity,
    vs_allocator *allocator,
    vs_hash_common_entry_destroy_fn entry_destroy
) {
    VSTD_ASSERT(buckets != NULL, "fatal: vs_hash_common_buckets_destroy invalid arguments");
    VSTD_ASSERT(entry_destroy != NULL, "fatal: vs_hash_common_buckets_destroy invalid arguments");

    for (size_t i = 0; i < capacity; i++) {
        vs_linked_list_node *node = buckets[i].head;
        while (node != NULL) {
            vs_linked_list_node *next = node->next;
            entry_destroy(node, allocator);
            node = next;
        }
    }

    vs_dealloc(allocator, (void *)buckets);
}

void vs_hash_common_bucket_pushfront(vs_hash_common_bucket *bucket, vs_linked_list_node *node) {
    VSTD_ASSERT(bucket != NULL, "fatal: vs_hash_common_bucket_pushfront invalid arguments");
    VSTD_ASSERT(node != NULL, "fatal: vs_hash_common_bucket_pushfront invalid arguments");

    node->next = bucket->head;
    bucket->head = node;
}

vs_linked_list_node *vs_hash_common_bucket_head(const vs_hash_common_bucket *bucket) {
    VSTD_ASSERT(bucket != NULL, "fatal: vs_hash_common_bucket_head invalid arguments");

    return bucket->head;
}
