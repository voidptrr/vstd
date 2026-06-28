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

#ifndef K4C_BUFFER_CURSOR_H
#define K4C_BUFFER_CURSOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "k4c/error.h"

typedef struct k4c_buf_cursor {
    const uint8_t *data;
    size_t len;
    size_t pos;
} k4c_buf_cursor;

typedef bool (*k4c_buf_cursor_predicate_fn)(void *context, uint8_t byte);

/* Create a non-owning cursor over data[0..len). */
k4c_buf_cursor k4c_buf_cursor_create(const void *data, size_t len);

/* Create a non-owning cursor over the bytes before text's terminating NUL. */
k4c_buf_cursor k4c_buf_cursor_create_from_cstr(const char *text);

/* Return whether cursor has no remaining bytes. */
bool k4c_buf_cursor_at_end(const k4c_buf_cursor *cursor);

/* Return the current zero-based offset from the start of the buffer. */
size_t k4c_buf_cursor_position(const k4c_buf_cursor *cursor);

/* Return the number of bytes from the current position to the end. */
size_t k4c_buf_cursor_remaining(const k4c_buf_cursor *cursor);

/* Return the byte at the current position without advancing. */
k4c_status k4c_buf_cursor_peek(const k4c_buf_cursor *cursor, uint8_t *out);

/* Return the byte offset bytes after the current position without advancing. */
k4c_status k4c_buf_cursor_peek_at(const k4c_buf_cursor *cursor, size_t offset, uint8_t *out);

/* Return whether the remaining buffer begins with bytes[0..len). */
bool k4c_buf_cursor_starts_with(const k4c_buf_cursor *cursor, const void *bytes, size_t len);

/* Return the current byte and advance by one byte. */
k4c_status k4c_buf_cursor_next(k4c_buf_cursor *cursor, uint8_t *out);

/* Advance by count bytes. */
k4c_status k4c_buf_cursor_skip(k4c_buf_cursor *cursor, size_t count);

/* Advance by one byte when the current byte equals expected. */
bool k4c_buf_cursor_next_if(k4c_buf_cursor *cursor, uint8_t expected);

/* Advance while predicate returns true and return the number of bytes skipped. */
size_t k4c_buf_cursor_skip_while(
    k4c_buf_cursor *cursor,
    k4c_buf_cursor_predicate_fn predicate,
    void *context
);

/* Advance past ASCII whitespace bytes. */
void k4c_buf_cursor_skip_ascii_whitespace(k4c_buf_cursor *cursor);

#endif
