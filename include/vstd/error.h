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

#ifndef VSTD_ERROR_H
#define VSTD_ERROR_H

#if defined(__GNUC__) || defined(__clang__)
#define VS_NODISCARD __attribute__((warn_unused_result))
#else
#define VS_NODISCARD
#endif

#define VS_RETURN_IF_ERROR(expr) \
    do { \
        vs_status status__ = (expr); \
        if (status__ != VS_STATUS_OK) { \
            return status__; \
        } \
    } while (0)

typedef enum vs_status {
    VS_STATUS_OK = 0,
    VS_STATUS_INVALID_ARGUMENT,
    VS_STATUS_NO_MEMORY,
    VS_STATUS_OVERFLOW,
    VS_STATUS_OUT_OF_RANGE,
    VS_STATUS_NOT_FOUND,
    VS_STATUS_EOF,
    VS_STATUS_IO,
    VS_STATUS_INVALID_DATA,
    VS_STATUS_UNSUPPORTED,
} vs_status;

const char *vs_status_message(vs_status status);

#endif
