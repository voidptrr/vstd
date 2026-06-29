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

#ifndef K4C_IO_READER_H
#define K4C_IO_READER_H

#include <stddef.h>
#include <stdint.h>

#include "k4c/buffer/cursor.h"
#include "k4c/error.h"

typedef struct k4c_reader k4c_reader;
typedef struct k4c_reader_vtable k4c_reader_vtable;

typedef k4c_status (*k4c_reader_read_fn)(
    void *context,
    uint8_t *data,
    size_t capacity,
    size_t *out_len
);

struct k4c_reader {
    void *ctx;
    const k4c_reader_vtable *vtable;
    uint8_t *data;
    size_t pos;
    size_t len;
    size_t capacity;
};

struct k4c_reader_vtable {
    k4c_reader_read_fn read;
};

/* Create a non-owning buffered reader over ctx using vtable for source reads. */
k4c_reader k4c_reader_create(
    void *ctx,
    const k4c_reader_vtable *vtable,
    uint8_t *data,
    size_t capacity
);

/* Take and return the next byte from reader. */
k4c_status k4c_reader_take_byte(k4c_reader *reader, uint8_t *out);

/*
 * Take bytes through delimiter and return them as a cursor.
 * Implementations decide how to represent a final unterminated segment.
 */
k4c_status k4c_reader_take_delimiter(k4c_reader *reader, uint8_t delimiter, k4c_buf_cursor *out);

#endif
