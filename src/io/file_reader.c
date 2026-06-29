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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "k4c/assert.h"
#include "k4c/buffer/cursor.h"
#include "k4c/error.h"
#include "k4c/io/file_reader.h"

static bool k4c_file_reader_mode_is_valid(k4c_file_reader_mode mode) {
    return mode == K4C_FILE_READER_MODE_BYTES || mode == K4C_FILE_READER_MODE_LINE;
}

static void k4c_file_reader_reset(k4c_file_reader *reader) {
    K4C_ASSERT(reader != NULL, "fatal: k4c_file_reader_reset invalid arguments");

    reader->mode = K4C_FILE_READER_MODE_BYTES;
    reader->file = NULL;
    reader->data = NULL;
    reader->len = 0;
    reader->buffer_capacity = 0;
}

k4c_status k4c_file_reader_init(
    k4c_file_reader *reader,
    FILE *file,
    k4c_file_reader_mode mode,
    uint8_t *data,
    size_t buffer_capacity
) {
    K4C_ASSERT(reader != NULL, "fatal: k4c_file_reader_init invalid arguments");

    k4c_file_reader_reset(reader);
    if (!k4c_file_reader_mode_is_valid(mode) || file == NULL || data == NULL
        || buffer_capacity == 0) {
        return K4C_STATUS_INVALID_ARGUMENT;
    }

    reader->mode = mode;
    reader->file = file;
    reader->data = data;
    reader->len = 0;
    reader->buffer_capacity = buffer_capacity;
    return K4C_STATUS_OK;
}

static k4c_status k4c_file_reader_next_bytes(k4c_file_reader *reader, k4c_buf_cursor *out) {
    reader->len = fread(reader->data, 1, reader->buffer_capacity, reader->file);
    if (reader->len > 0) {
        *out = k4c_buf_cursor_create(reader->data, reader->len);
        return K4C_STATUS_OK;
    }
    if (ferror(reader->file)) {
        return K4C_STATUS_IO;
    }
    return K4C_STATUS_EOF;
}

static k4c_status k4c_file_reader_next_line(k4c_file_reader *reader, k4c_buf_cursor *out) {
    reader->len = 0;
    int ch = 0;

    while (reader->len < reader->buffer_capacity && ch != '\n') {
        ch = fgetc(reader->file);
        if (ch == EOF) {
            if (ferror(reader->file)) {
                return K4C_STATUS_IO;
            }
            if (reader->len == 0) {
                return K4C_STATUS_EOF;
            }
            *out = k4c_buf_cursor_create(reader->data, reader->len);
            return K4C_STATUS_OK;
        }
        reader->data[reader->len] = (uint8_t)ch;
        reader->len++;
    }

    if (ch == '\n') {
        *out = k4c_buf_cursor_create(reader->data, reader->len);
        return K4C_STATUS_OK;
    }
    return K4C_STATUS_OVERFLOW;
}

k4c_status k4c_file_reader_next(k4c_file_reader *reader, k4c_buf_cursor *out) {
    K4C_ASSERT(reader != NULL, "fatal: k4c_file_reader_next invalid arguments");
    K4C_ASSERT(out != NULL, "fatal: k4c_file_reader_next invalid arguments");
    K4C_ASSERT(reader->file != NULL, "fatal: k4c_file_reader_next invalid arguments");
    K4C_ASSERT(reader->data != NULL, "fatal: k4c_file_reader_next invalid arguments");
    K4C_ASSERT(reader->buffer_capacity > 0, "fatal: k4c_file_reader_next invalid arguments");

    switch (reader->mode) {
        case K4C_FILE_READER_MODE_BYTES:
            return k4c_file_reader_next_bytes(reader, out);
        case K4C_FILE_READER_MODE_LINE:
            return k4c_file_reader_next_line(reader, out);
    }
    return K4C_STATUS_INVALID_ARGUMENT;
}

k4c_status k4c_file_reader_close(k4c_file_reader *reader) {
    K4C_ASSERT(reader != NULL, "fatal: k4c_file_reader_close invalid arguments");

    k4c_file_reader_reset(reader);
    return K4C_STATUS_OK;
}
