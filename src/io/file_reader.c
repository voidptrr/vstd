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

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "k4c/assert.h"
#include "k4c/error.h"
#include "k4c/io/file_reader.h"
#include "k4c/io/reader.h"

static k4c_status k4c_file_reader_read(
    void *context,
    uint8_t *data,
    size_t capacity,
    size_t *out_len
);

static const k4c_reader_vtable k4c_file_reader_vtable = {
    .read = k4c_file_reader_read,
};

static void k4c_file_reader_reset(k4c_file_reader *reader) {
    K4C_ASSERT(reader != NULL, "fatal: k4c_file_reader_reset invalid arguments");

    reader->file = NULL;
    reader->data = NULL;
    reader->buffer_capacity = 0;
}

static k4c_status k4c_file_reader_read(
    void *context,
    uint8_t *data,
    size_t capacity,
    size_t *out_len
) {
    k4c_file_reader *reader = context;

    K4C_ASSERT(reader != NULL, "fatal: k4c_file_reader_read invalid arguments");
    K4C_ASSERT(data != NULL, "fatal: k4c_file_reader_read invalid arguments");
    K4C_ASSERT(capacity > 0, "fatal: k4c_file_reader_read invalid arguments");
    K4C_ASSERT(out_len != NULL, "fatal: k4c_file_reader_read invalid arguments");
    K4C_ASSERT(reader->file != NULL, "fatal: k4c_file_reader_read invalid arguments");

    *out_len = fread(data, 1, capacity, reader->file);
    if (*out_len > 0) {
        return K4C_STATUS_OK;
    }
    if (ferror(reader->file)) {
        return K4C_STATUS_IO;
    }
    return K4C_STATUS_EOF;
}

k4c_status k4c_file_reader_create(
    k4c_file_reader *reader,
    FILE *file,
    uint8_t *data,
    size_t buffer_capacity
) {
    K4C_ASSERT(reader != NULL, "fatal: k4c_file_reader_create invalid arguments");

    k4c_file_reader_reset(reader);
    if (file == NULL || data == NULL || buffer_capacity == 0) {
        return K4C_STATUS_INVALID_ARGUMENT;
    }

    reader->file = file;
    reader->data = data;
    reader->buffer_capacity = buffer_capacity;
    return K4C_STATUS_OK;
}

k4c_reader k4c_file_reader_view(k4c_file_reader *reader) {
    K4C_ASSERT(reader != NULL, "fatal: k4c_file_reader_view invalid arguments");

    return k4c_reader_create(
        reader,
        &k4c_file_reader_vtable,
        reader->data,
        reader->buffer_capacity
    );
}

k4c_status k4c_file_reader_close(k4c_file_reader *reader) {
    K4C_ASSERT(reader != NULL, "fatal: k4c_file_reader_close invalid arguments");

    k4c_file_reader_reset(reader);
    return K4C_STATUS_OK;
}
