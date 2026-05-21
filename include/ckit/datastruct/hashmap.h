#ifndef CKIT_DATASTRUCT_HASHMAP_H
#define CKIT_DATASTRUCT_HASHMAP_H

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
typedef ckit_eq_fn ckit_hashmap_key_eq_fn;

typedef struct ckit_hashmap ckit_hashmap;

/*
 * Initialize a hash map with fixed key/value sizes.
 * Hashing uses the internal FNV-1a implementation.
 * Initial capacity is implementation-defined.
 */
ckit_hashmap *ckit_hashmap_init(size_t key_size, size_t value_size, ckit_hashmap_key_eq_fn key_eq,
                                ckit_allocator *allocator);

/*
 * Insert or update an entry.
 * If key exists, overwrite value in place.
 */
void ckit_hashmap_put(ckit_hashmap *map, const void *key, const void *value);

/*
 * Lookup key and return stored value pointer, or NULL when missing.
 */
const void *ckit_hashmap_get(const ckit_hashmap *map, const void *key);

/*
 * Remove key from map.
 */
void *ckit_hashmap_remove(ckit_hashmap *map, const void *key);

/*
 * Release all entries, bucket storage, and the hashmap handle.
 */
void ckit_hashmap_free(ckit_hashmap *map);

/* Return the number of stored entries. */
size_t ckit_hashmap_size(const ckit_hashmap *map);

/* Return whether map has zero entries. */
bool ckit_hashmap_is_empty(const ckit_hashmap *map);

#endif
