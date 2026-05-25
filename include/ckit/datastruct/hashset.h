#ifndef CKIT_DATASTRUCT_HASHSET_H
#define CKIT_DATASTRUCT_HASHSET_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/compare.h"
#include "ckit/memory/allocators/allocator.h"

/*
 * Opaque hash set with unique elements.
 *
 * Logical view:
 *
 *   buckets
 *   +---+     +--------+    +--------+
 *   | 0 | --> | elem   | -> | elem   | -> NULL
 *   +---+     +--------+    +--------+
 *   | 1 | --> NULL
 *   +---+
 *   | 2 | --> +--------+
 *   +---+     | elem   | -> NULL
 *   | . |     +--------+
 *   | . |
 *   +---+
 *
 * Elements are hashed into buckets. Collisions are resolved by walking the
 * chain for that bucket. Duplicate elements are not inserted.
 */

/* Element equality callback used to resolve collisions and lookups. */
typedef ckit_eq_fn ckit_hashset_elem_eq_fn;

typedef struct ckit_hashset ckit_hashset;

/*
 * Initialize a hash set with fixed element size.
 * Hashing uses the internal FNV-1a implementation.
 * Initial capacity is implementation-defined.
 */
ckit_hashset *ckit_hashset_init(size_t elem_size, ckit_hashset_elem_eq_fn elem_eq,
                                ckit_allocator *allocator);

/*
 * Insert element when it is not already present.
 * Existing elements are left unchanged.
 */
void ckit_hashset_insert(ckit_hashset *set, const void *elem);

/*
 * Return true when element exists in set.
 */
bool ckit_hashset_contains(const ckit_hashset *set, const void *elem);

/*
 * Lookup element and return stored element pointer, or NULL when missing.
 */
void *ckit_hashset_get(ckit_hashset *set, const void *elem);

/*
 * Lookup element and return const stored element pointer, or NULL when missing.
 */
const void *ckit_hashset_get_const(const ckit_hashset *set, const void *elem);

/*
 * Remove element from set.
 */
void ckit_hashset_remove(ckit_hashset *set, const void *elem);

/* Return the number of stored elements. */
size_t ckit_hashset_size(const ckit_hashset *set);

/*
 * Release all entries and the hashset handle.
 */
void ckit_hashset_deinit(ckit_hashset *set);

#endif
