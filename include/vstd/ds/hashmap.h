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

#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>
#include <stddef.h>

#include "vstd/ds/iterator.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"

#define hashmap_for_each_entry(item, map) \
    for (iterator item##_iter__ = hashmap_get_iterator((map), HASHMAP_ITERATOR_ENTRY); \
         ((item) = ITER_NEXT_AS(hashmap_entry_view, &item##_iter__)) != NULL;)

#define hashmap_for_each_key(type, item, map) \
    for (iterator item##_iter__ = hashmap_get_iterator((map), HASHMAP_ITERATOR_KEY); \
         ((item) = ITER_NEXT_AS(type, &item##_iter__)) != NULL;)

#define hashmap_for_each_value(type, item, map) \
    for (iterator item##_iter__ = hashmap_get_iterator((map), HASHMAP_ITERATOR_VALUE); \
         ((item) = ITER_NEXT_AS(type, &item##_iter__)) != NULL;)

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
typedef bool (*hashmap_key_eq_fn)(const void *lhs, const void *rhs);

typedef struct hashmap hashmap;
typedef struct linked_list_node linked_list_node;

typedef struct hashmap_entry_view {
    const void *key;
    const void *value;
} hashmap_entry_view;

typedef enum hashmap_iterator_type {
    HASHMAP_ITERATOR_ENTRY,
    HASHMAP_ITERATOR_KEY,
    HASHMAP_ITERATOR_VALUE,
} hashmap_iterator_type;

/*
 * Create a hash map with fixed key/value sizes.
 * Hashing uses the internal FNV-1a implementation over stored key bytes. When
 * key_eq is NULL, key equality compares stored key bytes.
 * Initial capacity is implementation-defined.
 */
NODISCARD status hashmap_create(
    size_t key_size,
    size_t value_size,
    hashmap_key_eq_fn key_eq,
    allocator *allocator,
    hashmap **out
);

/* Ensure map can hold at least size entries without growing. */
NODISCARD status hashmap_reserve(hashmap *map, size_t size);

/*
 * Insert or update an entry.
 * If key exists, overwrite value in place.
 */
NODISCARD status hashmap_put(hashmap *map, const void *key, const void *value);

/*
 * Lookup key and return stored value pointer, or NULL when missing.
 */
void *hashmap_get(hashmap *map, const void *key);

/*
 * Lookup key and return const stored value pointer, or NULL when missing.
 */
const void *hashmap_get_const(const hashmap *map, const void *key);

/*
 * Remove key from map.
 */
void hashmap_remove(hashmap *map, const void *key);

/* Return the number of stored entries. */
size_t hashmap_size(const hashmap *map);

/* Return an iterator over entries, keys, or values in bucket order. */
iterator hashmap_get_iterator(const hashmap *map, hashmap_iterator_type type);

/*
 * Release all entries, bucket storage, and the hashmap handle.
 */
void hashmap_destroy(hashmap *map);

#endif
