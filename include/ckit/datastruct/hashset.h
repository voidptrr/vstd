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

#ifndef CK_DATASTRUCT_HASHSET_H
#define CK_DATASTRUCT_HASHSET_H

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
typedef ck_eq_fn ck_hashset_elem_eq_fn;

typedef struct ck_hashset ck_hashset;

/*
 * Create a hash set with fixed element size.
 * Hashing uses the internal FNV-1a implementation.
 * Initial capacity is implementation-defined.
 */
ck_hashset *ck_hashset_create(size_t elem_size, ck_hashset_elem_eq_fn elem_eq,
                              ck_allocator *allocator);

/*
 * Insert element when it is not already present.
 * Existing elements are left unchanged.
 */
void ck_hashset_insert(ck_hashset *set, const void *elem);

/*
 * Return true when element exists in set.
 */
bool ck_hashset_contains(const ck_hashset *set, const void *elem);

/*
 * Lookup element and return stored element pointer, or NULL when missing.
 */
void *ck_hashset_get(ck_hashset *set, const void *elem);

/*
 * Lookup element and return const stored element pointer, or NULL when missing.
 */
const void *ck_hashset_get_const(const ck_hashset *set, const void *elem);

/*
 * Remove element from set.
 */
void ck_hashset_remove(ck_hashset *set, const void *elem);

/* Return the number of stored elements. */
size_t ck_hashset_size(const ck_hashset *set);

/*
 * Release all entries and the hashset handle.
 */
void ck_hashset_destroy(ck_hashset *set);

#endif
