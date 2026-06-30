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

#ifndef K4C_MEMORY_SIZE_H
#define K4C_MEMORY_SIZE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static inline size_t k4c_align_up(size_t value, size_t alignment) {
    size_t mask = alignment - 1;
    return (value + mask) & ~mask;
}

static inline bool k4c_size_add_overflow(size_t lhs, size_t rhs, size_t *out) {
    if (lhs > SIZE_MAX - rhs) {
        return true;
    }

    *out = lhs + rhs;
    return false;
}

static inline bool k4c_size_mul_overflow(size_t lhs, size_t rhs, size_t *out) {
    if (rhs != 0 && lhs > SIZE_MAX / rhs) {
        return true;
    }

    *out = lhs * rhs;
    return false;
}

static inline bool k4c_align_up_overflow(size_t value, size_t alignment, size_t *out) {
    size_t mask = alignment - 1;
    if (k4c_size_add_overflow(value, mask, out)) {
        return true;
    }

    *out &= ~mask;
    return false;
}

#endif
