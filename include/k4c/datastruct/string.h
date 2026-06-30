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

#ifndef K4C_STRING_H
#define K4C_STRING_H

#include <stdbool.h>
#include <stddef.h>

#include "k4c/allocators/allocator.h"
#include "k4c/datastruct/iterator.h"
#include "k4c/error.h"

#define k4c_string_for_each_char(item, k4c_string) \
    for (k4c_iterator item##_iter__ = k4c_string_get_iterator((k4c_string)); \
         item##_iter__.next != NULL; \
         item##_iter__.next = NULL) \
    k4c_iterator_for_each(char, item, &item##_iter__)

/*
 * Opaque growable k4c_string.
 *
 * Public view:
 *
 *   k4c_string
 *        |
 *        v
 *      "hello\0"
 *
 * Internal storage keeps metadata before the visible character buffer:
 *
 *   +----------------------+----------------------+-------------+
 *   | length / capacity    | k4c_allocator metadata   | char buf[]  |
 *   +----------------------+----------------------+-------------+
 *                                                 ^
 *                                                 |
 *                                            k4c_string
 */
typedef char *k4c_string;

/* Create a k4c_string from initial, or an empty k4c_string when initial is NULL. */
k4c_status k4c_string_create(const char *initial, k4c_allocator *k4c_allocator, k4c_string *out);

/* Append suffix to k4c_string, growing storage as needed. */
k4c_status k4c_string_append(k4c_string *k4c_string, const char *suffix);

/* Prepend prefix to k4c_string, growing storage as needed. */
k4c_status k4c_string_prepend(k4c_string *k4c_string, const char *prefix);

/* Return whether k4c_string contains needle. */
bool k4c_string_contains(const k4c_string k4c_string, const char *needle);

/* Return whether k4c_string begins with prefix. */
bool k4c_string_starts_with(const k4c_string k4c_string, const char *prefix);

/* Return whether k4c_string ends with suffix. */
bool k4c_string_ends_with(const k4c_string k4c_string, const char *suffix);

/* Reset k4c_string to empty without releasing storage. */
void k4c_string_clear(k4c_string k4c_string);

/* Return the number of bytes before the terminating NUL. */
size_t k4c_string_len(const k4c_string k4c_string);

/* Return an k4c_iterator over bytes before the terminating NUL. */
k4c_iterator k4c_string_get_iterator(const k4c_string k4c_string);

/* Destroy and release owned storage. */
void k4c_string_destroy(k4c_string k4c_string);

#endif
