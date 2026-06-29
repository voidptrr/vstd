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

#include <stdint.h>
#include <string.h>

#include "k4c/assert.h"
#include "k4c/buffer/cursor.h"
#include "k4c/error.h"
#include "k4c/io/reader.h"

static k4c_status k4c_reader_fill_more(k4c_reader *reader) {
    if (reader->pos > 0 && reader->pos < reader->len) {
        reader->len -= reader->pos;
        memmove(reader->data, reader->data + reader->pos, reader->len);
        reader->pos = 0;
    } else if (reader->pos >= reader->len) {
        reader->pos = 0;
        reader->len = 0;
    }

    if (reader->len == reader->capacity) {
        return K4C_STATUS_OVERFLOW;
    }

    size_t read_len = 0;
    k4c_status st = reader->vtable->read(
        reader->ctx,
        reader->data + reader->len,
        reader->capacity - reader->len,
        &read_len
    );
    K4C_ASSERT(
        read_len <= reader->capacity - reader->len,
        "fatal: k4c_reader_fill_more invalid read result"
    );

    reader->len += read_len;
    if (read_len > 0) {
        return K4C_STATUS_OK;
    }
    if (st == K4C_STATUS_OK) {
        return K4C_STATUS_EOF;
    }
    return st;
}

k4c_reader k4c_reader_create(
    void *ctx,
    const k4c_reader_vtable *vtable,
    uint8_t *data,
    size_t capacity
) {
    K4C_ASSERT(vtable != NULL, "fatal: k4c_reader_create invalid arguments");
    K4C_ASSERT(vtable->read != NULL, "fatal: k4c_reader_create invalid arguments");
    K4C_ASSERT(data != NULL, "fatal: k4c_reader_create invalid arguments");
    K4C_ASSERT(capacity > 0, "fatal: k4c_reader_create invalid arguments");

    return (k4c_reader){
        .ctx = ctx,
        .vtable = vtable,
        .data = data,
        .pos = 0,
        .len = 0,
        .capacity = capacity,
    };
}

k4c_status k4c_reader_take_byte(k4c_reader *reader, uint8_t *out) {
    K4C_ASSERT(reader != NULL, "fatal: k4c_reader_take_byte invalid arguments");
    K4C_ASSERT(reader->vtable != NULL, "fatal: k4c_reader_take_byte invalid arguments");
    K4C_ASSERT(reader->vtable->read != NULL, "fatal: k4c_reader_take_byte invalid arguments");
    K4C_ASSERT(reader->data != NULL, "fatal: k4c_reader_take_byte invalid arguments");
    K4C_ASSERT(reader->capacity > 0, "fatal: k4c_reader_take_byte invalid arguments");
    K4C_ASSERT(out != NULL, "fatal: k4c_reader_take_byte invalid arguments");

    if (reader->pos >= reader->len) {
        k4c_status st = k4c_reader_fill_more(reader);
        if (st != K4C_STATUS_OK) {
            return st;
        }
    }

    *out = reader->data[reader->pos];
    reader->pos += 1;
    return K4C_STATUS_OK;
}

k4c_status k4c_reader_take_delimiter(k4c_reader *reader, uint8_t delimiter, k4c_buf_cursor *out) {
    K4C_ASSERT(reader != NULL, "fatal: k4c_reader_take_delimiter invalid arguments");
    K4C_ASSERT(reader->vtable != NULL, "fatal: k4c_reader_take_delimiter invalid arguments");
    K4C_ASSERT(reader->vtable->read != NULL, "fatal: k4c_reader_take_delimiter invalid arguments");
    K4C_ASSERT(reader->data != NULL, "fatal: k4c_reader_take_delimiter invalid arguments");
    K4C_ASSERT(reader->capacity > 0, "fatal: k4c_reader_take_delimiter invalid arguments");
    K4C_ASSERT(out != NULL, "fatal: k4c_reader_take_delimiter invalid arguments");

    while (true) {
        uint8_t *start = reader->data + reader->pos;
        size_t available = reader->len - reader->pos;
        uint8_t *found = memchr(start, delimiter, available);
        if (found != NULL) {
            size_t out_len = (size_t)(found - start) + 1;
            *out = k4c_buf_cursor_create(start, out_len);
            reader->pos += out_len;
            return K4C_STATUS_OK;
        }

        if (available == reader->capacity) {
            return K4C_STATUS_OVERFLOW;
        }

        k4c_status st = k4c_reader_fill_more(reader);
        if (st == K4C_STATUS_EOF) {
            available = reader->len - reader->pos;
            if (available == 0) {
                return K4C_STATUS_EOF;
            }

            *out = k4c_buf_cursor_create(reader->data + reader->pos, available);
            reader->pos = reader->len;
            return K4C_STATUS_OK;
        }
        if (st != K4C_STATUS_OK) {
            return st;
        }
    }
}
