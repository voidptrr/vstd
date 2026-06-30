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

#ifndef K4C_HASHMAP_H
#define K4C_HASHMAP_H

#include <stdbool.h>
#include <stddef.h>

#include "k4c/allocators/allocator.h"
#include "k4c/datastruct/iterator.h"
#include "k4c/error.h"

#define k4c_hashmap_for_each_entry(item, map) \
    for (k4c_iterator item##_iter__ = k4c_hashmap_get_iterator((map), K4C_HASHMAP_ITERATOR_ENTRY); \
         ((item) = K4C_ITER_NEXT_AS(k4c_hashmap_entry_view, &item##_iter__)) != NULL;)

#define k4c_hashmap_for_each_key(type, item, map) \
    for (k4c_iterator item##_iter__ = k4c_hashmap_get_iterator((map), K4C_HASHMAP_ITERATOR_KEY); \
         ((item) = K4C_ITER_NEXT_AS(type, &item##_iter__)) != NULL;)

#define k4c_hashmap_for_each_value(type, item, map) \
    for (k4c_iterator item##_iter__ = k4c_hashmap_get_iterator((map), K4C_HASHMAP_ITERATOR_VALUE); \
         ((item) = K4C_ITER_NEXT_AS(type, &item##_iter__)) != NULL;)

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
typedef bool (*k4c_hashmap_key_eq_fn)(const void *lhs, const void *rhs);

typedef struct k4c_hashmap k4c_hashmap;
typedef struct k4c_linked_list_node k4c_linked_list_node;

typedef struct k4c_hashmap_entry_view {
    const void *key;
    const void *value;
} k4c_hashmap_entry_view;

typedef enum k4c_hashmap_iterator_type {
    K4C_HASHMAP_ITERATOR_ENTRY,
    K4C_HASHMAP_ITERATOR_KEY,
    K4C_HASHMAP_ITERATOR_VALUE,
} k4c_hashmap_iterator_type;

/*
 * Create a hash map with fixed key/value sizes.
 * Hashing uses the internal FNV-1a implementation over stored key bytes. When
 * key_eq is NULL, key equality compares stored key bytes.
 * Initial capacity is implementation-defined.
 */
k4c_status k4c_hashmap_create(
    size_t key_size,
    size_t value_size,
    k4c_hashmap_key_eq_fn key_eq,
    k4c_allocator *k4c_allocator,
    k4c_hashmap **out
);

/* Ensure map can hold at least size entries without growing. */
k4c_status k4c_hashmap_reserve(k4c_hashmap *map, size_t size);

/*
 * Insert or update an entry.
 * If key exists, overwrite value in place.
 */
k4c_status k4c_hashmap_put(k4c_hashmap *map, const void *key, const void *value);

/*
 * Lookup key and return stored value pointer, or NULL when missing.
 */
void *k4c_hashmap_get(const k4c_hashmap *map, const void *key);

/*
 * Remove key from map.
 */
void k4c_hashmap_remove(k4c_hashmap *map, const void *key);

/* Return the number of stored entries. */
size_t k4c_hashmap_size(const k4c_hashmap *map);

/* Return an k4c_iterator over entries, keys, or values in bucket order. */
k4c_iterator k4c_hashmap_get_iterator(const k4c_hashmap *map, k4c_hashmap_iterator_type type);

/*
 * Release all entries, bucket storage, and the k4c_hashmap handle.
 */
void k4c_hashmap_destroy(k4c_hashmap *map);

#endif
