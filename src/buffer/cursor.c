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

static bool k4c_buf_cursor_is_ascii_whitespace(void *context, uint8_t byte) {
    (void)context;

    return byte == ' ' || byte == '\t' || byte == '\n' || byte == '\r' || byte == '\f'
           || byte == '\v';
}

k4c_buf_cursor k4c_buf_cursor_create(const void *data, size_t len) {
    K4C_ASSERT(data != NULL || len == 0, "fatal: k4c_buf_cursor_create invalid arguments");

    return (k4c_buf_cursor){
        .data = (const uint8_t *)data,
        .len = len,
        .pos = 0,
    };
}

k4c_buf_cursor k4c_buf_cursor_create_from_cstr(const char *text) {
    K4C_ASSERT(text != NULL, "fatal: k4c_buf_cursor_create_from_cstr invalid arguments");

    return k4c_buf_cursor_create(text, strlen(text));
}

bool k4c_buf_cursor_is_eof(const k4c_buf_cursor *cursor) {
    K4C_ASSERT(cursor != NULL, "fatal: k4c_buf_cursor_is_eof invalid arguments");

    return cursor->pos >= cursor->len;
}

size_t k4c_buf_cursor_position(const k4c_buf_cursor *cursor) {
    K4C_ASSERT(cursor != NULL, "fatal: k4c_buf_cursor_position invalid arguments");

    return cursor->pos;
}

size_t k4c_buf_cursor_remaining(const k4c_buf_cursor *cursor) {
    K4C_ASSERT(cursor != NULL, "fatal: k4c_buf_cursor_remaining invalid arguments");

    if (cursor->pos >= cursor->len) {
        return 0;
    }
    return cursor->len - cursor->pos;
}

k4c_status k4c_buf_cursor_peek(const k4c_buf_cursor *cursor, uint8_t *out) {
    return k4c_buf_cursor_peek_at(cursor, 0, out);
}

k4c_status k4c_buf_cursor_peek_at(const k4c_buf_cursor *cursor, size_t offset, uint8_t *out) {
    K4C_ASSERT(cursor != NULL, "fatal: k4c_buf_cursor_peek_at invalid arguments");
    K4C_ASSERT(out != NULL, "fatal: k4c_buf_cursor_peek_at invalid arguments");

    if (offset >= k4c_buf_cursor_remaining(cursor)) {
        return K4C_STATUS_EOF;
    }

    *out = cursor->data[cursor->pos + offset];
    return K4C_STATUS_OK;
}

bool k4c_buf_cursor_starts_with(const k4c_buf_cursor *cursor, const void *bytes, size_t len) {
    K4C_ASSERT(cursor != NULL, "fatal: k4c_buf_cursor_starts_with invalid arguments");
    K4C_ASSERT(bytes != NULL || len == 0, "fatal: k4c_buf_cursor_starts_with invalid arguments");

    if (len > k4c_buf_cursor_remaining(cursor)) {
        return false;
    }

    return memcmp(cursor->data + cursor->pos, bytes, len) == 0;
}

k4c_status k4c_buf_cursor_next(k4c_buf_cursor *cursor, uint8_t *out) {
    K4C_ASSERT(cursor != NULL, "fatal: k4c_buf_cursor_next invalid arguments");

    K4C_RETURN_IF_ERROR(k4c_buf_cursor_peek(cursor, out));
    cursor->pos += 1;
    return K4C_STATUS_OK;
}

k4c_status k4c_buf_cursor_skip(k4c_buf_cursor *cursor, size_t count) {
    K4C_ASSERT(cursor != NULL, "fatal: k4c_buf_cursor_skip invalid arguments");

    if (count > k4c_buf_cursor_remaining(cursor)) {
        return K4C_STATUS_EOF;
    }

    cursor->pos += count;
    return K4C_STATUS_OK;
}

bool k4c_buf_cursor_next_if(k4c_buf_cursor *cursor, uint8_t expected) {
    K4C_ASSERT(cursor != NULL, "fatal: k4c_buf_cursor_next_if invalid arguments");

    uint8_t byte = 0;
    if (k4c_buf_cursor_peek(cursor, &byte) != K4C_STATUS_OK || byte != expected) {
        return false;
    }

    cursor->pos += 1;
    return true;
}

size_t k4c_buf_cursor_skip_while(
    k4c_buf_cursor *cursor,
    k4c_buf_cursor_predicate_fn predicate,
    void *context
) {
    K4C_ASSERT(cursor != NULL, "fatal: k4c_buf_cursor_skip_while invalid arguments");
    K4C_ASSERT(predicate != NULL, "fatal: k4c_buf_cursor_skip_while invalid arguments");

    size_t start = cursor->pos;
    while (!k4c_buf_cursor_is_eof(cursor)) {
        uint8_t byte = 0;
        if (k4c_buf_cursor_peek(cursor, &byte) != K4C_STATUS_OK || !predicate(context, byte)) {
            break;
        }

        cursor->pos += 1;
    }

    return cursor->pos - start;
}

void k4c_buf_cursor_skip_ascii_whitespace(k4c_buf_cursor *cursor) {
    K4C_ASSERT(cursor != NULL, "fatal: k4c_buf_cursor_skip_ascii_whitespace invalid arguments");

    (void)k4c_buf_cursor_skip_while(cursor, k4c_buf_cursor_is_ascii_whitespace, NULL);
}
