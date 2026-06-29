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
#include <string.h>

#include "common.h"
#include "k4c/buffer/cursor.h"
#include "k4c/error.h"
#include "k4c/io/file.h"
#include "k4c/io/file_reader.h"
#include "k4c/io/reader.h"
#include "k4c/testing.h"

static int close_reader_and_return(k4c_file_reader *reader, int result) {
    if (k4c_file_reader_close(reader) != K4C_STATUS_OK) {
        return 1;
    }
    return result;
}

static int cleanup_reader_stream_file_dir_and_return(
    k4c_file_reader *reader,
    FILE *file,
    const char *file_path,
    const char *dir,
    int result
) {
    if (close_reader_and_return(reader, 0) != 0) {
        result = 1;
    }
    return k4c_io_test_cleanup_stream_file_dir_and_return(file, file_path, dir, result);
}

static int cursor_equal(k4c_buf_cursor *cursor, const char *text) {
    size_t len = strlen(text);
    if (k4c_test_equal(cursor->len, len) != 0) {
        return 1;
    }
    if (memcmp(cursor->data, text, len) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(create_initializes_reader) {
    char dir[256];
    if (k4c_io_test_make_temp_dir(dir, sizeof(dir), "k4c-file-reader-test") != 0) {
        return 1;
    }

    char file_path[256];
    if (k4c_io_test_make_temp_file_path(file_path, sizeof(file_path), dir, "data.bin") != 0) {
        return k4c_io_test_cleanup_dir_and_return(dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_write_all(file_path, "abc", 3)) != 0) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }
    uint8_t data[64];
    k4c_file_reader reader;
    if (k4c_test_status_ok(k4c_file_reader_create(&reader, file, data, sizeof(data))) != 0) {
        return k4c_io_test_cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }

    k4c_reader generic = k4c_file_reader_view(&reader);
    if (k4c_test_equal_ptr(generic.ctx, &reader) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal_ptr(reader.file, file) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal_ptr(reader.data, data) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(reader.buffer_capacity, 64) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal_ptr(generic.data, data) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(generic.pos, 0) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(generic.len, 0) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(generic.capacity, 64) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }

    return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 0);
}

K4C_TEST(create_rejects_null_file) {
    k4c_file_reader reader;
    uint8_t data[64];
    if (k4c_test_equal(
            k4c_file_reader_create(&reader, NULL, data, sizeof(data)),
            K4C_STATUS_INVALID_ARGUMENT
        )
        != 0) {
        return 1;
    }
    if (k4c_test_equal(reader.file == NULL, true) != 0) {
        return 1;
    }
    if (k4c_test_equal(reader.data == NULL, true) != 0) {
        return 1;
    }
    if (k4c_test_equal(reader.buffer_capacity, 0) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(create_rejects_zero_buffer_capacity) {
    k4c_file_reader reader;
    uint8_t data[1];
    if (k4c_test_equal(k4c_file_reader_create(&reader, stdin, data, 0), K4C_STATUS_INVALID_ARGUMENT)
        != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(close_resets_reader_without_closing_stream) {
    char dir[256];
    if (k4c_io_test_make_temp_dir(dir, sizeof(dir), "k4c-file-reader-test") != 0) {
        return 1;
    }

    char file_path[256];
    if (k4c_io_test_make_temp_file_path(file_path, sizeof(file_path), dir, "data.bin") != 0) {
        return k4c_io_test_cleanup_dir_and_return(dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_write_all(file_path, "abc", 3)) != 0) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }
    uint8_t data[64];
    k4c_file_reader reader;
    if (k4c_test_status_ok(k4c_file_reader_create(&reader, file, data, sizeof(data))) != 0) {
        return k4c_io_test_cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }
    k4c_reader generic = k4c_file_reader_view(&reader);
    if (k4c_test_equal_ptr(generic.ctx, &reader) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }

    if (k4c_test_status_ok(k4c_file_reader_close(&reader)) != 0) {
        return k4c_io_test_cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }
    if (k4c_test_equal(reader.file == NULL, true) != 0) {
        return k4c_io_test_cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }
    if (k4c_test_equal(reader.data == NULL, true) != 0) {
        return k4c_io_test_cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }
    if (k4c_test_equal(reader.buffer_capacity, 0) != 0) {
        return k4c_io_test_cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_reader_close(&reader)) != 0) {
        return k4c_io_test_cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }
    if (k4c_test_equal(fgetc(file), 'a') != 0) {
        return k4c_io_test_cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }

    return k4c_io_test_cleanup_stream_file_dir_and_return(file, file_path, dir, 0);
}

K4C_TEST(create_returns_reader_that_takes_bytes) {
    char dir[256];
    if (k4c_io_test_make_temp_dir(dir, sizeof(dir), "k4c-file-reader-test") != 0) {
        return 1;
    }

    char file_path[256];
    if (k4c_io_test_make_temp_file_path(file_path, sizeof(file_path), dir, "data.bin") != 0) {
        return k4c_io_test_cleanup_dir_and_return(dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_write_all(file_path, "abc", 3)) != 0) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }
    uint8_t data[4];
    k4c_file_reader file_reader;
    if (k4c_test_status_ok(k4c_file_reader_create(&file_reader, file, data, sizeof(data))) != 0) {
        return k4c_io_test_cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }
    k4c_reader reader = k4c_file_reader_view(&file_reader);

    uint8_t byte = 0;
    if (k4c_test_status_ok(k4c_reader_take_byte(&reader, &byte)) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&file_reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(byte, 'a') != 0) {
        return cleanup_reader_stream_file_dir_and_return(&file_reader, file, file_path, dir, 1);
    }
    if (k4c_test_status_ok(k4c_reader_take_byte(&reader, &byte)) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&file_reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(byte, 'b') != 0) {
        return cleanup_reader_stream_file_dir_and_return(&file_reader, file, file_path, dir, 1);
    }
    if (k4c_test_status_ok(k4c_reader_take_byte(&reader, &byte)) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&file_reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(byte, 'c') != 0) {
        return cleanup_reader_stream_file_dir_and_return(&file_reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(k4c_reader_take_byte(&reader, &byte), K4C_STATUS_EOF) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&file_reader, file, file_path, dir, 1);
    }

    return cleanup_reader_stream_file_dir_and_return(&file_reader, file, file_path, dir, 0);
}

K4C_TEST(create_returns_reader_that_takes_delimiters) {
    char dir[256];
    if (k4c_io_test_make_temp_dir(dir, sizeof(dir), "k4c-file-reader-test") != 0) {
        return 1;
    }

    char file_path[256];
    if (k4c_io_test_make_temp_file_path(file_path, sizeof(file_path), dir, "data.txt") != 0) {
        return k4c_io_test_cleanup_dir_and_return(dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_write_all(file_path, "one,two,three", 13)) != 0) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }
    uint8_t data[8];
    k4c_file_reader file_reader;
    if (k4c_test_status_ok(k4c_file_reader_create(&file_reader, file, data, sizeof(data))) != 0) {
        return k4c_io_test_cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }
    k4c_reader reader = k4c_file_reader_view(&file_reader);

    k4c_buf_cursor chunk;
    if (k4c_test_status_ok(k4c_reader_take_delimiter(&reader, ',', &chunk)) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&file_reader, file, file_path, dir, 1);
    }
    if (cursor_equal(&chunk, "one,") != 0) {
        return cleanup_reader_stream_file_dir_and_return(&file_reader, file, file_path, dir, 1);
    }
    if (k4c_test_status_ok(k4c_reader_take_delimiter(&reader, ',', &chunk)) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&file_reader, file, file_path, dir, 1);
    }
    if (cursor_equal(&chunk, "two,") != 0) {
        return cleanup_reader_stream_file_dir_and_return(&file_reader, file, file_path, dir, 1);
    }
    if (k4c_test_status_ok(k4c_reader_take_delimiter(&reader, ',', &chunk)) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&file_reader, file, file_path, dir, 1);
    }
    if (cursor_equal(&chunk, "three") != 0) {
        return cleanup_reader_stream_file_dir_and_return(&file_reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(k4c_reader_take_delimiter(&reader, ',', &chunk), K4C_STATUS_EOF) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&file_reader, file, file_path, dir, 1);
    }

    return cleanup_reader_stream_file_dir_and_return(&file_reader, file, file_path, dir, 0);
}

K4C_TEST(create_reader_rejects_delimiters_over_buffer_capacity) {
    char dir[256];
    if (k4c_io_test_make_temp_dir(dir, sizeof(dir), "k4c-file-reader-test") != 0) {
        return 1;
    }

    char file_path[256];
    if (k4c_io_test_make_temp_file_path(file_path, sizeof(file_path), dir, "data.txt") != 0) {
        return k4c_io_test_cleanup_dir_and_return(dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_write_all(file_path, "abcdef\n", 7)) != 0) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }
    uint8_t data[4];
    k4c_file_reader reader;
    if (k4c_test_status_ok(k4c_file_reader_create(&reader, file, data, sizeof(data))) != 0) {
        return k4c_io_test_cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }
    k4c_reader generic = k4c_file_reader_view(&reader);

    k4c_buf_cursor cursor;
    if (k4c_test_equal(k4c_reader_take_delimiter(&generic, '\n', &cursor), K4C_STATUS_OVERFLOW)
        != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(generic.len, 4) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(generic.pos, 0) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }

    return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 0);
}

K4C_TEST(create_reader_rejects_final_delimited_chunk_at_buffer_capacity) {
    char dir[256];
    if (k4c_io_test_make_temp_dir(dir, sizeof(dir), "k4c-file-reader-test") != 0) {
        return 1;
    }

    char file_path[256];
    if (k4c_io_test_make_temp_file_path(file_path, sizeof(file_path), dir, "data.txt") != 0) {
        return k4c_io_test_cleanup_dir_and_return(dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_write_all(file_path, "abcd", 4)) != 0) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }
    uint8_t data[4];
    k4c_file_reader reader;
    if (k4c_test_status_ok(k4c_file_reader_create(&reader, file, data, sizeof(data))) != 0) {
        return k4c_io_test_cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }
    k4c_reader generic = k4c_file_reader_view(&reader);

    k4c_buf_cursor cursor;
    if (k4c_test_equal(k4c_reader_take_delimiter(&generic, '\n', &cursor), K4C_STATUS_OVERFLOW)
        != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(generic.len, 4) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(generic.pos, 0) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }

    return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 0);
}

K4C_TEST_MAIN(
    K4C_TEST_CASE(create_initializes_reader),
    K4C_TEST_CASE(create_rejects_null_file),
    K4C_TEST_CASE(create_rejects_zero_buffer_capacity),
    K4C_TEST_CASE(close_resets_reader_without_closing_stream),
    K4C_TEST_CASE(create_returns_reader_that_takes_bytes),
    K4C_TEST_CASE(create_returns_reader_that_takes_delimiters),
    K4C_TEST_CASE(create_reader_rejects_delimiters_over_buffer_capacity),
    K4C_TEST_CASE(create_reader_rejects_final_delimited_chunk_at_buffer_capacity)
)
