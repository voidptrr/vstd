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

#ifndef VSTD_HASHSET_H
#define VSTD_HASHSET_H

#include <stdbool.h>
#include <stddef.h>

#include "vstd/memory/allocators/allocator.h"

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
typedef bool (*vs_hashset_elem_eq_fn)(const void *lhs, const void *rhs);

typedef struct vs_hashset vs_hashset;

/*
 * Create a hash set with fixed element size.
 * Hashing uses the internal FNV-1a implementation over stored element bytes.
 * When elem_eq is NULL, element equality compares stored element bytes.
 * Initial capacity is implementation-defined.
 */
vs_hashset *vs_hashset_create(
    size_t elem_size,
    vs_hashset_elem_eq_fn elem_eq,
    vs_allocator *allocator
);

/*
 * Insert element when it is not already present.
 * Existing elements are left unchanged.
 */
void vs_hashset_insert(vs_hashset *set, const void *elem);

/*
 * Return true when element exists in set.
 */
bool vs_hashset_contains(const vs_hashset *set, const void *elem);

/*
 * Lookup element and return stored element pointer, or NULL when missing.
 */
void *vs_hashset_get(vs_hashset *set, const void *elem);

/*
 * Lookup element and return const stored element pointer, or NULL when missing.
 */
const void *vs_hashset_get_const(const vs_hashset *set, const void *elem);

/*
 * Remove element from set.
 */
void vs_hashset_remove(vs_hashset *set, const void *elem);

/* Return the number of stored elements. */
size_t vs_hashset_size(const vs_hashset *set);

/*
 * Release all entries and the hashset handle.
 */
void vs_hashset_destroy(vs_hashset *set);

#endif
