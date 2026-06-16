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

#include <stdint.h>
#include <string.h>

#include "vstd/compare.h"

bool vs_eq_bytes(const void *lhs, const void *rhs, size_t size) {
    return memcmp(lhs, rhs, size) == 0;
}

bool vs_eq_cstr(const void *lhs, const void *rhs, size_t size) {
    (void)size;
    return strcmp((const char *)lhs, (const char *)rhs) == 0;
}

bool vs_eq_cstr_ptr(const void *lhs, const void *rhs, size_t size) {
    (void)size;
    const char *a = *(const char *const *)lhs;
    const char *b = *(const char *const *)rhs;
    return strcmp(a, b) == 0;
}

bool vs_eq_i32(const void *lhs, const void *rhs, size_t size) {
    (void)size;
    return (*(const int32_t *)lhs) == (*(const int32_t *)rhs);
}

bool vs_eq_i64(const void *lhs, const void *rhs, size_t size) {
    (void)size;
    return (*(const int64_t *)lhs) == (*(const int64_t *)rhs);
}

bool vs_eq_u32(const void *lhs, const void *rhs, size_t size) {
    (void)size;
    return (*(const uint32_t *)lhs) == (*(const uint32_t *)rhs);
}

bool vs_eq_u64(const void *lhs, const void *rhs, size_t size) {
    (void)size;
    return (*(const uint64_t *)lhs) == (*(const uint64_t *)rhs);
}

bool vs_eq_size(const void *lhs, const void *rhs, size_t size) {
    (void)size;
    return (*(const size_t *)lhs) == (*(const size_t *)rhs);
}

int vs_cmp_i32(const void *lhs, const void *rhs) {
    int32_t a = *(const int32_t *)lhs;
    int32_t b = *(const int32_t *)rhs;
    return (a > b) - (a < b);
}

int vs_cmp_i64(const void *lhs, const void *rhs) {
    int64_t a = *(const int64_t *)lhs;
    int64_t b = *(const int64_t *)rhs;
    return (a > b) - (a < b);
}

int vs_cmp_u32(const void *lhs, const void *rhs) {
    uint32_t a = *(const uint32_t *)lhs;
    uint32_t b = *(const uint32_t *)rhs;
    return (a > b) - (a < b);
}

int vs_cmp_u64(const void *lhs, const void *rhs) {
    uint64_t a = *(const uint64_t *)lhs;
    uint64_t b = *(const uint64_t *)rhs;
    return (a > b) - (a < b);
}

int vs_cmp_size(const void *lhs, const void *rhs) {
    size_t a = *(const size_t *)lhs;
    size_t b = *(const size_t *)rhs;
    return (a > b) - (a < b);
}
