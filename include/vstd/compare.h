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

#ifndef VSTD_COMPARE_H
#define VSTD_COMPARE_H

#include <stdbool.h>
#include <stddef.h>

/* Shared callback shapes used by generic containers. */
typedef bool (*vs_eq_fn)(const void *lhs, const void *rhs, size_t size);
typedef int (*vs_cmp_fn)(const void *lhs, const void *rhs);

/*
 * Shared equality helpers for key matching (hashmap key_eq callbacks).
 * vs_eq_bytes honors size. vs_eq_cstr compares inline null-terminated
 * character data. vs_eq_cstr_ptr compares stored const char * values.
 * Scalar helpers ignore size. Pointer-string equality requires a matching hash
 * over string contents when used with hash-based containers.
 */
bool vs_eq_bytes(const void *lhs, const void *rhs, size_t size);
bool vs_eq_cstr(const void *lhs, const void *rhs, size_t size);
bool vs_eq_cstr_ptr(const void *lhs, const void *rhs, size_t size);
bool vs_eq_i32(const void *lhs, const void *rhs, size_t size);
bool vs_eq_i64(const void *lhs, const void *rhs, size_t size);
bool vs_eq_u32(const void *lhs, const void *rhs, size_t size);
bool vs_eq_u64(const void *lhs, const void *rhs, size_t size);
bool vs_eq_size(const void *lhs, const void *rhs, size_t size);

/*
 * Shared ordering helpers for container callbacks that use int-style
 * comparators (for example binary heap and future ordered utilities).
 */
int vs_cmp_i32(const void *lhs, const void *rhs);
int vs_cmp_i64(const void *lhs, const void *rhs);
int vs_cmp_u32(const void *lhs, const void *rhs);
int vs_cmp_u64(const void *lhs, const void *rhs);
int vs_cmp_size(const void *lhs, const void *rhs);

#endif
