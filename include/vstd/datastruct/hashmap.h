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

#ifndef VSTD_HASHMAP_H
#define VSTD_HASHMAP_H

#include <stdbool.h>
#include <stddef.h>

#include "vstd/datastruct/iterator.h"
#include "vstd/memory/allocator.h"

#define VS_HASHMAP_FOR_EACH_ENTRY(item, map) \
    for (vs_iterator item##_vs_iter__ = vs_hashmap_get_iterator((map), VS_HASHMAP_ITERATOR_ENTRY); \
         ((item) = VS_ITER_NEXT_AS(vs_hashmap_entry_view, &item##_vs_iter__)) != NULL;)

#define VS_HASHMAP_FOR_EACH_KEY(type, item, map) \
    for (vs_iterator item##_vs_iter__ = vs_hashmap_get_iterator((map), VS_HASHMAP_ITERATOR_KEY); \
         ((item) = VS_ITER_NEXT_AS(type, &item##_vs_iter__)) != NULL;)

#define VS_HASHMAP_FOR_EACH_VALUE(type, item, map) \
    for (vs_iterator item##_vs_iter__ = vs_hashmap_get_iterator((map), VS_HASHMAP_ITERATOR_VALUE); \
         ((item) = VS_ITER_NEXT_AS(type, &item##_vs_iter__)) != NULL;)

/*
 * Opaque hash map with separate chaining.
 *
 * Logical view:
 *
 *   buckets
 *   +---+     +-------------+    +-------------+
 *   | 0 | --> | key | value | -> | key | value | -> NULL
 *   +---+     +-------------+    +-------------+
 *   | 1 | --> NULL
 *   +---+
 *   | 2 | --> +-------------+
 *   +---+     | key | value | -> NULL
 *   | . |     +-------------+
 *   | . |
 *   +---+
 *
 * Keys are hashed into buckets. Collisions are resolved by walking the chain
 * for that bucket.
 */

/* Key equality callback used to resolve collisions and lookups. */
typedef bool (*vs_hashmap_key_eq_fn)(const void *lhs, const void *rhs);

typedef struct vs_hashmap vs_hashmap;
typedef struct vs_linked_list_node vs_linked_list_node;

typedef struct vs_hashmap_entry_view {
    const void *key;
    const void *value;
} vs_hashmap_entry_view;

typedef enum vs_hashmap_iterator_type {
    VS_HASHMAP_ITERATOR_ENTRY,
    VS_HASHMAP_ITERATOR_KEY,
    VS_HASHMAP_ITERATOR_VALUE,
} vs_hashmap_iterator_type;

/*
 * Create a hash map with fixed key/value sizes.
 * Hashing uses the internal FNV-1a implementation over stored key bytes. When
 * key_eq is NULL, key equality compares stored key bytes.
 * Initial capacity is implementation-defined.
 */
vs_hashmap *vs_hashmap_create(
    size_t key_size,
    size_t value_size,
    vs_hashmap_key_eq_fn key_eq,
    vs_allocator *allocator
);

/* Ensure map can hold at least size entries without growing. */
void vs_hashmap_reserve(vs_hashmap *map, size_t size);

/*
 * Insert or update an entry.
 * If key exists, overwrite value in place.
 */
void vs_hashmap_put(vs_hashmap *map, const void *key, const void *value);

/*
 * Lookup key and return stored value pointer, or NULL when missing.
 */
void *vs_hashmap_get(vs_hashmap *map, const void *key);

/*
 * Lookup key and return const stored value pointer, or NULL when missing.
 */
const void *vs_hashmap_get_const(const vs_hashmap *map, const void *key);

/*
 * Remove key from map.
 */
void vs_hashmap_remove(vs_hashmap *map, const void *key);

/* Return the number of stored entries. */
size_t vs_hashmap_size(const vs_hashmap *map);

/* Return an iterator over entries, keys, or values in bucket order. */
vs_iterator vs_hashmap_get_iterator(const vs_hashmap *map, vs_hashmap_iterator_type type);

/*
 * Release all entries, bucket storage, and the hashmap handle.
 */
void vs_hashmap_destroy(vs_hashmap *map);

#endif
