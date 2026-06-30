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

#ifndef K4C_HASHSET_H
#define K4C_HASHSET_H

#include <stdbool.h>
#include <stddef.h>

#include "k4c/allocators/allocator.h"
#include "k4c/datastruct/iterator.h"
#include "k4c/error.h"

#define k4c_hashset_for_each(type, item, set) \
    for (k4c_iterator item##_iter__ = k4c_hashset_get_iterator((set)); \
         ((item) = K4C_ITER_NEXT_AS(type, &item##_iter__)) != NULL;)

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
typedef bool (*k4c_hashset_elem_eq_fn)(const void *lhs, const void *rhs);

typedef struct k4c_hashset k4c_hashset;
typedef struct k4c_linked_list_node k4c_linked_list_node;

/*
 * Create a hash set with fixed element size.
 * Hashing uses the internal FNV-1a implementation over stored element bytes.
 * When elem_eq is NULL, element equality compares stored element bytes.
 * Initial capacity is implementation-defined.
 */
k4c_status k4c_hashset_create(
    size_t elem_size,
    k4c_hashset_elem_eq_fn elem_eq,
    k4c_allocator *k4c_allocator,
    k4c_hashset **out
);

/* Ensure set can hold at least size elements without growing. */
k4c_status k4c_hashset_reserve(k4c_hashset *set, size_t size);

/*
 * Insert element when it is not already present.
 * Existing elements are left unchanged.
 */
k4c_status k4c_hashset_insert(k4c_hashset *set, const void *elem);

/*
 * Return true when element exists in set.
 */
bool k4c_hashset_contains(const k4c_hashset *set, const void *elem);

/*
 * Lookup element and return stored element pointer, or NULL when missing.
 */
void *k4c_hashset_get(const k4c_hashset *set, const void *elem);

/*
 * Remove element from set.
 */
void k4c_hashset_remove(k4c_hashset *set, const void *elem);

/* Return the number of stored elements. */
size_t k4c_hashset_size(const k4c_hashset *set);

/* Return an k4c_iterator over stored elements in bucket order. */
k4c_iterator k4c_hashset_get_iterator(const k4c_hashset *set);

/*
 * Release all entries and the k4c_hashset handle.
 */
void k4c_hashset_destroy(k4c_hashset *set);

#endif
