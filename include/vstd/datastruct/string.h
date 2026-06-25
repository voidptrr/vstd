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

#ifndef VSTD_STRING_H
#define VSTD_STRING_H

#include <stdbool.h>
#include <stddef.h>

#include "vstd/datastruct/iterator.h"
#include "vstd/memory/allocator.h"

/*
 * Opaque growable string.
 *
 * Public view:
 *
 *   vs_string
 *        |
 *        v
 *      "hello\0"
 *
 * Internal storage keeps metadata before the visible character buffer:
 *
 *   +----------------------+----------------------+-------------+
 *   | length / capacity    | allocator metadata   | char buf[]  |
 *   +----------------------+----------------------+-------------+
 *                                                 ^
 *                                                 |
 *                                            vs_string
 */
typedef char *vs_string;

#define VS_STRING_FOR_EACH_CHAR(item, string) \
    for (vs_iterator item##_vs_iter__ = vs_string_get_iterator((string)); \
         item##_vs_iter__.next != NULL; \
         item##_vs_iter__.next = NULL) \
    VS_ITERATOR_FOR_EACH(char, item, &item##_vs_iter__)

/* Create a string from initial, or an empty string when initial is NULL. */
vs_string vs_string_create(const char *initial, vs_allocator *allocator);

/* Append suffix to string, growing storage as needed. */
void vs_string_append(vs_string *string, const char *suffix);

/* Prepend prefix to string, growing storage as needed. */
void vs_string_prepend(vs_string *string, const char *prefix);

/* Return whether string contains needle. */
bool vs_string_contains(const vs_string string, const char *needle);

/* Return whether string begins with prefix. */
bool vs_string_starts_with(const vs_string string, const char *prefix);

/* Return whether string ends with suffix. */
bool vs_string_ends_with(const vs_string string, const char *suffix);

/* Reset string to empty without releasing storage. */
void vs_string_clear(vs_string string);

/* Return the number of bytes before the terminating NUL. */
size_t vs_string_len(const vs_string string);

/* Return an iterator over bytes before the terminating NUL. */
vs_iterator vs_string_get_iterator(const vs_string string);

/* Destroy and release owned storage. */
void vs_string_destroy(vs_string string);

#endif
