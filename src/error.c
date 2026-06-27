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

#include "vstd/error.h"

const char *vs_status_message(vs_status status) {
    switch (status) {
        case VS_STATUS_OK:
            return "ok";
        case VS_STATUS_INVALID_ARGUMENT:
            return "invalid argument";
        case VS_STATUS_NO_MEMORY:
            return "out of memory";
        case VS_STATUS_OVERFLOW:
            return "overflow";
        case VS_STATUS_OUT_OF_RANGE:
            return "out of range";
        case VS_STATUS_NOT_FOUND:
            return "not found";
        case VS_STATUS_EOF:
            return "end of file";
        case VS_STATUS_IO:
            return "I/O error";
        case VS_STATUS_INVALID_DATA:
            return "invalid data";
        case VS_STATUS_UNSUPPORTED:
            return "unsupported operation";
    }
    return "unknown error";
}
