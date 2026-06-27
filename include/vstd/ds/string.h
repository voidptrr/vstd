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

#ifndef STRING_H
#define STRING_H

#include <stdbool.h>
#include <stddef.h>

#include "vstd/ds/iterator.h"
#include "vstd/error.h"
#include "vstd/memory/allocator.h"

#define string_for_each_char(item, string) \
    for (iterator item##_iter__ = string_get_iterator((string)); item##_iter__.next != NULL; \
         item##_iter__.next = NULL) \
    iterator_for_each(char, item, &item##_iter__)

/*
 * Opaque growable string.
 *
 * Public view:
 *
 *   string
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
 *                                            string
 */
typedef char *string;

/* Create a string from initial, or an empty string when initial is NULL. */
NODISCARD status string_create(const char *initial, allocator *allocator, string *out);

/* Append suffix to string, growing storage as needed. */
NODISCARD status string_append(string *string, const char *suffix);

/* Prepend prefix to string, growing storage as needed. */
NODISCARD status string_prepend(string *string, const char *prefix);

/* Return whether string contains needle. */
bool string_contains(const string string, const char *needle);

/* Return whether string begins with prefix. */
bool string_starts_with(const string string, const char *prefix);

/* Return whether string ends with suffix. */
bool string_ends_with(const string string, const char *suffix);

/* Reset string to empty without releasing storage. */
void string_clear(string string);

/* Return the number of bytes before the terminating NUL. */
size_t string_len(const string string);

/* Return an iterator over bytes before the terminating NUL. */
iterator string_get_iterator(const string string);

/* Destroy and release owned storage. */
void string_destroy(string string);

#endif
