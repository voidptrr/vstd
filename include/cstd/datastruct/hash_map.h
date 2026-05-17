#ifndef CSTD_DATASTRUCT_HASH_MAP_H
#define CSTD_DATASTRUCT_HASH_MAP_H

#include <stdbool.h>
#include <stddef.h>

#include "cstd/status.h"

/*
 * Hash map with separate chaining.
 *
 * cstd_hash_map
 * +--------------------------------------------------------------------------+
 * | size | key_size | value_size | capacity | buckets | hash | key_eq        |
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

typedef size_t (*cstd_hash_map_hash_fn)(const void *key, size_t key_size);
typedef bool (*cstd_hash_map_key_eq_fn)(const void *lhs, const void *rhs, size_t key_size);

typedef struct cstd_hash_map_entry {
    void *key;
    void *value;
    struct cstd_hash_map_entry *next;
} cstd_hash_map_entry;

typedef struct cstd_hash_map {
    size_t size;
    size_t key_size;
    size_t value_size;
    size_t capacity;
    cstd_hash_map_entry **buckets;
    cstd_hash_map_hash_fn hash;
    cstd_hash_map_key_eq_fn key_eq;
} cstd_hash_map;

/*
 * Initialize a hash map with fixed key/value sizes and user callbacks.
 * Initial capacity is implementation-defined.
 */
cstd_status cstd_hash_map_init(cstd_hash_map *map, size_t key_size, size_t value_size,
                               cstd_hash_map_hash_fn hash, cstd_hash_map_key_eq_fn key_eq);

/*
 * Insert or update an entry.
 * If key exists, overwrite value in place.
 */
cstd_status cstd_hash_map_put(cstd_hash_map *map, const void *key, const void *value);

/*
 * Lookup key and copy value into out_value.
 */
cstd_status cstd_hash_map_get(const cstd_hash_map *map, const void *key, void *out_value);

/*
 * Remove key and optionally copy removed value into out_value when out_value != NULL.
 */
cstd_status cstd_hash_map_remove(cstd_hash_map *map, const void *key, void *out_value);

/*
 * Release all entries and bucket storage, reset map to empty state.
 */
cstd_status cstd_hash_map_free(cstd_hash_map *map);

/* Return the number of stored entries. */
size_t cstd_hash_map_size(const cstd_hash_map *map);

/* Return whether map has zero entries. */
bool cstd_hash_map_is_empty(const cstd_hash_map *map);

#endif
