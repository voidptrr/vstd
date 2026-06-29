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

#ifndef K4C_IO_FILE_READER_H
#define K4C_IO_FILE_READER_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "k4c/buffer/cursor.h"
#include "k4c/error.h"

typedef enum k4c_file_reader_mode {
    K4C_FILE_READER_MODE_BYTES,
    K4C_FILE_READER_MODE_LINE,
} k4c_file_reader_mode;

typedef struct k4c_file_reader {
    k4c_file_reader_mode mode;
    FILE *file;
    uint8_t *data;
    size_t len;
    /* Maximum bytes held in data for one read result. */
    size_t buffer_capacity;
} k4c_file_reader;

/*
 * Initialize reader from an existing FILE pointer and caller-provided storage.
 * The caller owns file and data. data must point to buffer_capacity bytes and
 * outlive the reader.
 * buffer_capacity is both the maximum chunk size and the maximum accepted line
 * length in line mode. Lines that fill the buffer without a newline overflow.
 */
k4c_status k4c_file_reader_init(
    k4c_file_reader *reader,
    FILE *file,
    k4c_file_reader_mode mode,
    uint8_t *data,
    size_t buffer_capacity
);

/*
 * Read the next chunk according to reader mode into reader-owned storage.
 * Line chunks include the terminating newline when present.
 */
k4c_status k4c_file_reader_next(k4c_file_reader *reader, k4c_buf_cursor *out);

/* Reset the reader. Caller-owned file and data are not released. */
k4c_status k4c_file_reader_close(k4c_file_reader *reader);

#endif
