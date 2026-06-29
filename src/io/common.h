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

#ifndef K4C_IO_COMMON_H
#define K4C_IO_COMMON_H

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>

#include "k4c/assert.h"
#include "k4c/error.h"

static inline bool k4c_io_errno_is_not_found(void) {
    return errno == ENOENT || errno == ENOTDIR;
}

static inline k4c_status k4c_io_file_close(FILE *file) {
    K4C_ASSERT(file != NULL, "fatal: k4c_io_file_close invalid arguments");

    return fclose(file) == 0 ? K4C_STATUS_OK : K4C_STATUS_IO;
}

static inline k4c_status k4c_io_preserve_status(k4c_status status, k4c_status cleanup_status) {
    return status != K4C_STATUS_OK ? status : cleanup_status;
}

#endif
