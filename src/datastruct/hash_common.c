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
#include "vstd/compare.h"
#include "vstd/datastruct/linked_list.h"
#include "vstd/memory/allocators/allocator.h"
#include "vstd/panic.h"

vs_linked_list **vs_hash_common_buckets_create(size_t capacity, vs_allocator *allocator) {
    VS_ASSERT(capacity > 0, "fatal: vs_hash_common_buckets_create invalid arguments");

    size_t alloc_size = sizeof(vs_linked_list *) * capacity;
    vs_linked_list **buckets = (vs_linked_list **)vs_malloc(allocator, alloc_size);
    memset((void *)buckets, 0, alloc_size);

    for (size_t i = 0; i < capacity; i++) {
        buckets[i] = vs_linked_list_create(allocator);
    }

    return buckets;
}

vs_linked_list **vs_hash_common_buckets_rehash(
    vs_linked_list **buckets,
    size_t capacity,
    size_t new_capacity,
    size_t value_size,
    vs_allocator *allocator,
    vs_hash_common_entry_value_fn entry_value
) {
    VS_ASSERT(buckets != NULL, "fatal: vs_hash_common_buckets_rehash invalid arguments");
    VS_ASSERT(capacity > 0, "fatal: vs_hash_common_buckets_rehash invalid arguments");
    VS_ASSERT(new_capacity > 0, "fatal: vs_hash_common_buckets_rehash invalid arguments");
    VS_ASSERT(value_size > 0, "fatal: vs_hash_common_buckets_rehash invalid arguments");
    VS_ASSERT(entry_value != NULL, "fatal: vs_hash_common_buckets_rehash invalid arguments");

    vs_linked_list **new_buckets = vs_hash_common_buckets_create(new_capacity, allocator);
    for (size_t i = 0; i < capacity; i++) {
        vs_linked_list_node *node = vs_linked_list_head(buckets[i]);
        while (node != NULL) {
            vs_linked_list_node *next = node->next;
            const void *value = entry_value(node);
            size_t bucket = vs_hash_common_bucket_index(value, value_size, new_capacity);

            vs_linked_list_pushfront(new_buckets[bucket], node);
            node = next;
        }
        vs_linked_list_destroy(buckets[i]);
    }

    vs_dealloc(allocator, (void *)buckets);
    return new_buckets;
}

vs_linked_list_node *vs_hash_common_bucket_find(
    vs_linked_list *bucket,
    const void *value,
    size_t value_size,
    vs_eq_fn value_eq,
    vs_hash_common_entry_value_fn entry_value
) {
    VS_ASSERT(bucket != NULL, "fatal: vs_hash_common_bucket_find invalid arguments");
    VS_ASSERT(value != NULL, "fatal: vs_hash_common_bucket_find invalid arguments");
    VS_ASSERT(value_size > 0, "fatal: vs_hash_common_bucket_find invalid arguments");
    VS_ASSERT(value_eq != NULL, "fatal: vs_hash_common_bucket_find invalid arguments");
    VS_ASSERT(entry_value != NULL, "fatal: vs_hash_common_bucket_find invalid arguments");

    vs_linked_list_node *node = vs_linked_list_head(bucket);
    while (node != NULL) {
        if (value_eq(entry_value(node), value, value_size)) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

vs_linked_list_node *vs_hash_common_bucket_remove(
    vs_linked_list *bucket,
    const void *value,
    size_t value_size,
    vs_eq_fn value_eq,
    vs_hash_common_entry_value_fn entry_value
) {
    VS_ASSERT(bucket != NULL, "fatal: vs_hash_common_bucket_remove invalid arguments");
    VS_ASSERT(value != NULL, "fatal: vs_hash_common_bucket_remove invalid arguments");
    VS_ASSERT(value_size > 0, "fatal: vs_hash_common_bucket_remove invalid arguments");
    VS_ASSERT(value_eq != NULL, "fatal: vs_hash_common_bucket_remove invalid arguments");
    VS_ASSERT(entry_value != NULL, "fatal: vs_hash_common_bucket_remove invalid arguments");

    vs_linked_list_node *prev = NULL;
    vs_linked_list_node *node = vs_linked_list_head(bucket);
    while (node != NULL) {
        if (value_eq(entry_value(node), value, value_size)) {
            vs_linked_list_remove_after(bucket, prev);
            return node;
        }
        prev = node;
        node = node->next;
    }

    return NULL;
}

void vs_hash_common_buckets_destroy(
    vs_linked_list **buckets,
    size_t capacity,
    vs_allocator *allocator,
    vs_hash_common_entry_destroy_fn entry_destroy
) {
    VS_ASSERT(buckets != NULL, "fatal: vs_hash_common_buckets_destroy invalid arguments");
    VS_ASSERT(entry_destroy != NULL, "fatal: vs_hash_common_buckets_destroy invalid arguments");

    for (size_t i = 0; i < capacity; i++) {
        vs_linked_list_node *node = vs_linked_list_head(buckets[i]);
        while (node != NULL) {
            vs_linked_list_node *next = node->next;
            entry_destroy(node, allocator);
            node = next;
        }
        vs_linked_list_destroy(buckets[i]);
    }

    vs_dealloc(allocator, (void *)buckets);
}
