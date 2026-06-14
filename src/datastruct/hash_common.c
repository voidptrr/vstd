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

#include "ckit/compare.h"
#include "ckit/common/panic.h"
#include "ckit/datastruct/linked_list.h"
#include "ckit/memory/allocators/allocator.h"
#include "datastruct/hash_common.h"

ck_linked_list **ck_hash_common_buckets_create(size_t capacity, ck_allocator *allocator) {
    CK_ASSERT(capacity > 0, "fatal: ck_hash_common_buckets_create invalid arguments");

    size_t alloc_size = sizeof(ck_linked_list *) * capacity;
    ck_linked_list **buckets = (ck_linked_list **)ck_malloc(allocator, alloc_size);
    memset((void *)buckets, 0, alloc_size);

    for (size_t i = 0; i < capacity; i++) {
        buckets[i] = ck_linked_list_create(allocator);
    }

    return buckets;
}

ck_linked_list **ck_hash_common_buckets_rehash(ck_linked_list **buckets, size_t capacity,
                                               size_t new_capacity, size_t value_size,
                                               ck_allocator *allocator,
                                               ck_hash_common_entry_value_fn entry_value) {
    CK_ASSERT(buckets != NULL, "fatal: ck_hash_common_buckets_rehash invalid arguments");
    CK_ASSERT(capacity > 0, "fatal: ck_hash_common_buckets_rehash invalid arguments");
    CK_ASSERT(new_capacity > 0, "fatal: ck_hash_common_buckets_rehash invalid arguments");
    CK_ASSERT(value_size > 0, "fatal: ck_hash_common_buckets_rehash invalid arguments");
    CK_ASSERT(entry_value != NULL, "fatal: ck_hash_common_buckets_rehash invalid arguments");

    ck_linked_list **new_buckets = ck_hash_common_buckets_create(new_capacity, allocator);
    for (size_t i = 0; i < capacity; i++) {
        ck_linked_list_node *node = ck_linked_list_head(buckets[i]);
        while (node != NULL) {
            ck_linked_list_node *next = node->next;
            const void *value = entry_value(node);
            size_t bucket = ck_hash_common_bucket_index(value, value_size, new_capacity);

            ck_linked_list_pushfront(new_buckets[bucket], node);
            node = next;
        }
        ck_linked_list_destroy(buckets[i]);
    }

    ck_dealloc(allocator, (void *)buckets);
    return new_buckets;
}

ck_linked_list_node *ck_hash_common_bucket_find(ck_linked_list *bucket, const void *value,
                                                size_t value_size, ck_eq_fn value_eq,
                                                ck_hash_common_entry_value_fn entry_value) {
    CK_ASSERT(bucket != NULL, "fatal: ck_hash_common_bucket_find invalid arguments");
    CK_ASSERT(value != NULL, "fatal: ck_hash_common_bucket_find invalid arguments");
    CK_ASSERT(value_size > 0, "fatal: ck_hash_common_bucket_find invalid arguments");
    CK_ASSERT(value_eq != NULL, "fatal: ck_hash_common_bucket_find invalid arguments");
    CK_ASSERT(entry_value != NULL, "fatal: ck_hash_common_bucket_find invalid arguments");

    ck_linked_list_node *node = ck_linked_list_head(bucket);
    while (node != NULL) {
        if (value_eq(entry_value(node), value, value_size)) {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

ck_linked_list_node *ck_hash_common_bucket_remove(ck_linked_list *bucket, const void *value,
                                                  size_t value_size, ck_eq_fn value_eq,
                                                  ck_hash_common_entry_value_fn entry_value) {
    CK_ASSERT(bucket != NULL, "fatal: ck_hash_common_bucket_remove invalid arguments");
    CK_ASSERT(value != NULL, "fatal: ck_hash_common_bucket_remove invalid arguments");
    CK_ASSERT(value_size > 0, "fatal: ck_hash_common_bucket_remove invalid arguments");
    CK_ASSERT(value_eq != NULL, "fatal: ck_hash_common_bucket_remove invalid arguments");
    CK_ASSERT(entry_value != NULL, "fatal: ck_hash_common_bucket_remove invalid arguments");

    ck_linked_list_node *prev = NULL;
    ck_linked_list_node *node = ck_linked_list_head(bucket);
    while (node != NULL) {
        if (value_eq(entry_value(node), value, value_size)) {
            ck_linked_list_remove_after(bucket, prev);
            return node;
        }
        prev = node;
        node = node->next;
    }

    return NULL;
}

void ck_hash_common_buckets_destroy(ck_linked_list **buckets, size_t capacity,
                                    ck_allocator *allocator,
                                    ck_hash_common_entry_destroy_fn entry_destroy) {
    CK_ASSERT(buckets != NULL, "fatal: ck_hash_common_buckets_destroy invalid arguments");
    CK_ASSERT(entry_destroy != NULL, "fatal: ck_hash_common_buckets_destroy invalid arguments");

    for (size_t i = 0; i < capacity; i++) {
        ck_linked_list_node *node = ck_linked_list_head(buckets[i]);
        while (node != NULL) {
            ck_linked_list_node *next = node->next;
            entry_destroy(node, allocator);
            node = next;
        }
        ck_linked_list_destroy(buckets[i]);
    }

    ck_dealloc(allocator, (void *)buckets);
}
