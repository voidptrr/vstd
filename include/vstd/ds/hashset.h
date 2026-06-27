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

#ifndef HASHSET_H
#define HASHSET_H

#include <stdbool.h>
#include <stddef.h>

#include "vstd/ds/iterator.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"

#define hashset_for_each(type, item, set) \
    for (iterator item##_iter__ = hashset_get_iterator((set)); \
         ((item) = ITER_NEXT_AS(type, &item##_iter__)) != NULL;)

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
typedef bool (*hashset_elem_eq_fn)(const void *lhs, const void *rhs);

typedef struct hashset hashset;
typedef struct linked_list_node linked_list_node;

/*
 * Create a hash set with fixed element size.
 * Hashing uses the internal FNV-1a implementation over stored element bytes.
 * When elem_eq is NULL, element equality compares stored element bytes.
 * Initial capacity is implementation-defined.
 */
NODISCARD status
hashset_create(size_t elem_size, hashset_elem_eq_fn elem_eq, allocator *allocator, hashset **out);

/* Ensure set can hold at least size elements without growing. */
NODISCARD status hashset_reserve(hashset *set, size_t size);

/*
 * Insert element when it is not already present.
 * Existing elements are left unchanged.
 */
NODISCARD status hashset_insert(hashset *set, const void *elem);

/*
 * Return true when element exists in set.
 */
bool hashset_contains(const hashset *set, const void *elem);

/*
 * Lookup element and return stored element pointer, or NULL when missing.
 */
void *hashset_get(hashset *set, const void *elem);

/*
 * Lookup element and return const stored element pointer, or NULL when missing.
 */
const void *hashset_get_const(const hashset *set, const void *elem);

/*
 * Remove element from set.
 */
void hashset_remove(hashset *set, const void *elem);

/* Return the number of stored elements. */
size_t hashset_size(const hashset *set);

/* Return an iterator over stored elements in bucket order. */
iterator hashset_get_iterator(const hashset *set);

/*
 * Release all entries and the hashset handle.
 */
void hashset_destroy(hashset *set);

#endif
