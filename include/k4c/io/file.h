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

#ifndef K4C_IO_FILE_H
#define K4C_IO_FILE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "k4c/error.h"
#include "k4c/memory/allocator.h"

/* Return whether path exists and is a directory. */
k4c_status k4c_file_is_dir(const char *path, bool *out);

/* Return the byte size of a regular file. */
k4c_status k4c_file_size(const char *path, size_t *out);

/* Read the whole file into allocator-owned memory. */
k4c_status k4c_file_read_all(
    const char *path,
    k4c_allocator *k4c_allocator,
    uint8_t **out_data,
    size_t *out_len
);

/* Write data[0..len) to path, replacing any existing file. */
k4c_status k4c_file_write_all(const char *path, const void *data, size_t len);

#endif
