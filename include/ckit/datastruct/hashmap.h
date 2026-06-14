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

#ifndef CK_DATASTRUCT_HASHMAP_H
#define CK_DATASTRUCT_HASHMAP_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/compare.h"
#include "ckit/memory/allocators/allocator.h"

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
typedef ck_eq_fn ck_hashmap_key_eq_fn;

typedef struct ck_hashmap ck_hashmap;

/*
 * Create a hash map with fixed key/value sizes.
 * Hashing uses the internal FNV-1a implementation.
 * Initial capacity is implementation-defined.
 */
ck_hashmap *ck_hashmap_create(size_t key_size, size_t value_size, ck_hashmap_key_eq_fn key_eq,
                              ck_allocator *allocator);

/*
 * Insert or update an entry.
 * If key exists, overwrite value in place.
 */
void ck_hashmap_put(ck_hashmap *map, const void *key, const void *value);

/*
 * Lookup key and return stored value pointer, or NULL when missing.
 */
void *ck_hashmap_get(ck_hashmap *map, const void *key);

/*
 * Lookup key and return const stored value pointer, or NULL when missing.
 */
const void *ck_hashmap_get_const(const ck_hashmap *map, const void *key);

/*
 * Remove key from map.
 */
void ck_hashmap_remove(ck_hashmap *map, const void *key);

/* Return the number of stored entries. */
size_t ck_hashmap_size(const ck_hashmap *map);

/*
 * Release all entries, bucket storage, and the hashmap handle.
 */
void ck_hashmap_destroy(ck_hashmap *map);

#endif
