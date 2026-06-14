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

#ifndef CK_DATASTRUCT_STRING_H
#define CK_DATASTRUCT_STRING_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/memory/allocators/allocator.h"

/*
 * Opaque growable string.
 *
 * Public view:
 *
 *   ck_string
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
 *                                            ck_string
 */
typedef char *ck_string;

/* Create a string from initial, or an empty string when initial is NULL. */
ck_string ck_string_create(const char *initial, ck_allocator *allocator);

/* Append suffix to string, growing storage as needed. */
void ck_string_append(ck_string *string, const char *suffix);

/* Prepend prefix to string, growing storage as needed. */
void ck_string_prepend(ck_string *string, const char *prefix);

/* Return whether string contains needle. */
bool ck_string_contains(const ck_string string, const char *needle);

/* Return whether string begins with prefix. */
bool ck_string_starts_with(const ck_string string, const char *prefix);

/* Return whether string ends with suffix. */
bool ck_string_ends_with(const ck_string string, const char *suffix);

/* Reset string to empty without releasing storage. */
void ck_string_clear(ck_string string);

/* Return the number of bytes before the terminating NUL. */
size_t ck_string_len(const ck_string string);

/* Destroy and release owned storage. */
void ck_string_destroy(ck_string string);

#endif
