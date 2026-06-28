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

#include "k4c/buffer/cursor.h"
#include "k4c/error.h"
#include "k4c/testing.h"

static bool byte_in_set(void *context, uint8_t byte) {
    const char *set = context;

    for (size_t i = 0; set[i] != '\0'; i++) {
        if ((uint8_t)set[i] == byte) {
            return true;
        }
    }
    return false;
}

K4C_TEST(create_tracks_buffer) {
    const uint8_t bytes[] = {0, 1, 2};
    k4c_buf_cursor cursor = k4c_buf_cursor_create(bytes, sizeof(bytes));

    if (k4c_test_equal_ptr(cursor.data, bytes) != 0) {
        return 1;
    }
    if (k4c_test_equal(cursor.len, 3) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_position(&cursor), 0) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_remaining(&cursor), 3) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_at_end(&cursor), false) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(create_from_cstr_uses_length_before_nul) {
    k4c_buf_cursor cursor = k4c_buf_cursor_create_from_cstr("abc");

    if (k4c_test_equal(cursor.len, 3) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_remaining(&cursor), 3) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(peek_does_not_advance) {
    k4c_buf_cursor cursor = k4c_buf_cursor_create_from_cstr("abc");
    uint8_t byte = 0;

    if (k4c_test_equal(k4c_buf_cursor_peek(&cursor, &byte), K4C_STATUS_OK) != 0) {
        return 1;
    }
    if (k4c_test_equal(byte, 'a') != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_peek_at(&cursor, 2, &byte), K4C_STATUS_OK) != 0) {
        return 1;
    }
    if (k4c_test_equal(byte, 'c') != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_peek_at(&cursor, 3, &byte), K4C_STATUS_EOF) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_position(&cursor), 0) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(starts_with_checks_remaining_buffer) {
    k4c_buf_cursor cursor = k4c_buf_cursor_create_from_cstr("abc");

    if (k4c_test_equal(k4c_buf_cursor_starts_with(&cursor, "ab", 2), true) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_starts_with(&cursor, "ac", 2), false) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_starts_with(&cursor, "abcd", 4), false) != 0) {
        return 1;
    }
    if (k4c_test_status_ok(k4c_buf_cursor_skip(&cursor, 1)) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_starts_with(&cursor, "bc", 2), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(next_skip_and_next_if_advance) {
    k4c_buf_cursor cursor = k4c_buf_cursor_create_from_cstr("abc");
    uint8_t byte = 0;

    if (k4c_test_status_ok(k4c_buf_cursor_next(&cursor, &byte)) != 0) {
        return 1;
    }
    if (k4c_test_equal(byte, 'a') != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_next_if(&cursor, 'b'), true) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_next_if(&cursor, 'x'), false) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_position(&cursor), 2) != 0) {
        return 1;
    }
    if (k4c_test_status_ok(k4c_buf_cursor_skip(&cursor, 1)) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_at_end(&cursor), true) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_next(&cursor, &byte), K4C_STATUS_EOF) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_skip(&cursor, 1), K4C_STATUS_EOF) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(skip_while_uses_predicate) {
    k4c_buf_cursor cursor = k4c_buf_cursor_create_from_cstr("abc123");

    if (k4c_test_equal(k4c_buf_cursor_skip_while(&cursor, byte_in_set, "abc"), 3) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_position(&cursor), 3) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_skip_while(&cursor, byte_in_set, "0123456789"), 3) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_at_end(&cursor), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(skip_ascii_whitespace_uses_ascii_set) {
    k4c_buf_cursor cursor = k4c_buf_cursor_create_from_cstr(" \t\n\r\f\vabc");

    k4c_buf_cursor_skip_ascii_whitespace(&cursor);
    if (k4c_test_equal(k4c_buf_cursor_position(&cursor), 6) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_buf_cursor_starts_with(&cursor, "abc", 3), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST_MAIN(
    K4C_TEST_CASE(create_tracks_buffer),
    K4C_TEST_CASE(create_from_cstr_uses_length_before_nul),
    K4C_TEST_CASE(peek_does_not_advance),
    K4C_TEST_CASE(starts_with_checks_remaining_buffer),
    K4C_TEST_CASE(next_skip_and_next_if_advance),
    K4C_TEST_CASE(skip_while_uses_predicate),
    K4C_TEST_CASE(skip_ascii_whitespace_uses_ascii_set)
)
