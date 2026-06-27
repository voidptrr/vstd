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

#ifndef ERROR_H
#define ERROR_H

#if defined(__GNUC__) || defined(__clang__)
#define NODISCARD __attribute__((warn_unused_result))
#else
#define NODISCARD
#endif

#define RETURN_IF_ERROR(expr) \
    do { \
        status status__ = (expr); \
        if (status__ != STATUS_OK) { \
            return status__; \
        } \
    } while (0)

typedef enum status {
    STATUS_OK = 0,
    STATUS_INVALID_ARGUMENT,
    STATUS_NO_MEMORY,
    STATUS_OVERFLOW,
    STATUS_OUT_OF_RANGE,
    STATUS_NOT_FOUND,
    STATUS_EOF,
    STATUS_IO,
    STATUS_INVALID_DATA,
    STATUS_UNSUPPORTED,
} status;

const char *status_message(status status);

#endif
