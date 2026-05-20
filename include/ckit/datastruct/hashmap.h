#ifndef CKIT_DATASTRUCT_HASHMAP_H
#define CKIT_DATASTRUCT_HASHMAP_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/mem/allocators/allocator.h"

/*
 * Hash map with separate chaining.
 *
 * ckit_hashmap
 * +--------------------------------------------------------------------------+
 * | size | key_size | value_size | capacity | buckets | key_eq               |
 * +--------------------------------------------------------------------------+
 *                                            |
 *                                            v
 * buckets (array of entry-head pointers)
 * +---------+---------+---------+---------+---------+
 * | [0]     | [1]     | [2]     | [3]     | [4]     |
 * +----+----+----+----+----+----+----+----+----+----+
 *      |         |                   |
 *      v         v                   v
 *    NULL      +-------+      +-------+ -> +-------+ -> NULL
 *              | kA,vA |      | kB,vB |    | kC,vC |
 *              +---+---+      +---+---+    +---+---+
 *                  |              |
 *                  v              v
 *                NULL           NULL
 *
 * Example collision chain at one bucket:
 * buckets[3] -> [kB,vB] -> [kC,vC] -> NULL
 */

/* Key equality callback used to resolve collisions and lookups. */
typedef bool (*ckit_hashmap_key_eq_fn)(const void *lhs, const void *rhs, size_t key_size);

/* One chained entry stored inside a bucket list. */
typedef struct ckit_hashmap_entry {
    void *key;
    void *value;
    struct ckit_hashmap_entry *next;
} ckit_hashmap_entry;

/* Hash map object and its bucket array metadata. */
typedef struct ckit_hashmap {
    size_t size;
    size_t key_size;
    size_t value_size;
    size_t capacity;
    ckit_hashmap_entry **buckets;
    ckit_hashmap_key_eq_fn key_eq;
    ckit_allocator *allocator;
} ckit_hashmap;

/*
 * Initialize a hash map with fixed key/value sizes.
 * Hashing uses the internal FNV-1a implementation.
 * Initial capacity is implementation-defined.
 */
void ckit_hashmap_init(ckit_hashmap *map, size_t key_size, size_t value_size,
                       ckit_hashmap_key_eq_fn key_eq, ckit_allocator *allocator);

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
 * Release all entries and bucket storage, reset map to empty state.
 */
void ckit_hashmap_free(ckit_hashmap *map);

/* Return the number of stored entries. */
size_t ckit_hashmap_size(const ckit_hashmap *map);

/* Return whether map has zero entries. */
bool ckit_hashmap_is_empty(const ckit_hashmap *map);

#endif
