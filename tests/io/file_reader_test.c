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
#include <sys/stat.h>
#include <unistd.h>

#include "k4c/buffer/cursor.h"
#include "k4c/error.h"
#include "k4c/io/file.h"
#include "k4c/io/file_reader.h"
#include "k4c/testing.h"

static int make_temp_dir(char *out, size_t out_size) {
    static unsigned int counter;

    int written =
        snprintf(out, out_size, "/tmp/k4c-file-reader-test-%ld-%u", (long)getpid(), counter++);
    if (written < 0 || (size_t)written >= out_size) {
        return 1;
    }
    return mkdir(out, 0700) != 0 ? 1 : 0;
}

static int make_temp_file_path(char *out, size_t out_size, const char *dir, const char *name) {
    int written = snprintf(out, out_size, "%s/%s", dir, name);
    if (written < 0 || (size_t)written >= out_size) {
        return 1;
    }
    return 0;
}

static int close_reader_and_return(k4c_file_reader *reader, int result) {
    if (k4c_file_reader_close(reader) != K4C_STATUS_OK) {
        return 1;
    }
    return result;
}

static int cleanup_dir_and_return(const char *dir, int result) {
    if (rmdir(dir) != 0) {
        return 1;
    }
    return result;
}

static int cleanup_file_dir_and_return(const char *file_path, const char *dir, int result) {
    int cleanup_result = 0;

    if (unlink(file_path) != 0) {
        cleanup_result = 1;
    }
    if (rmdir(dir) != 0) {
        cleanup_result = 1;
    }

    return cleanup_result != 0 ? 1 : result;
}

static int cleanup_stream_file_dir_and_return(
    FILE *file,
    const char *file_path,
    const char *dir,
    int result
) {
    int cleanup_result = 0;

    if (file != NULL && fclose(file) != 0) {
        cleanup_result = 1;
    }
    if (cleanup_file_dir_and_return(file_path, dir, result) != result) {
        cleanup_result = 1;
    }

    return cleanup_result != 0 ? 1 : result;
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
    return cleanup_stream_file_dir_and_return(file, file_path, dir, result);
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

K4C_TEST(init_initializes_reader) {
    char dir[256];
    if (make_temp_dir(dir, sizeof(dir)) != 0) {
        return 1;
    }

    char file_path[256];
    if (make_temp_file_path(file_path, sizeof(file_path), dir, "data.bin") != 0) {
        return cleanup_dir_and_return(dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_write_all(file_path, "abc", 3)) != 0) {
        return cleanup_file_dir_and_return(file_path, dir, 1);
    }

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        return cleanup_file_dir_and_return(file_path, dir, 1);
    }
    uint8_t data[64];
    k4c_file_reader reader;
    if (k4c_test_status_ok(
            k4c_file_reader_init(&reader, file, K4C_FILE_READER_MODE_BYTES, data, sizeof(data))
        )
        != 0) {
        return cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }

    if (k4c_test_equal(reader.mode, K4C_FILE_READER_MODE_BYTES) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal_ptr(reader.file, file) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal_ptr(reader.data, data) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(reader.len, 0) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(reader.buffer_capacity, 64) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }

    return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 0);
}

K4C_TEST(init_rejects_null_file) {
    k4c_file_reader reader;
    uint8_t data[64];
    if (k4c_test_equal(
            k4c_file_reader_init(&reader, NULL, K4C_FILE_READER_MODE_BYTES, data, sizeof(data)),
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
    if (k4c_test_equal(reader.len, 0) != 0) {
        return 1;
    }
    if (k4c_test_equal(reader.buffer_capacity, 0) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(init_rejects_zero_buffer_capacity) {
    k4c_file_reader reader;
    uint8_t data[1];
    if (k4c_test_equal(
            k4c_file_reader_init(&reader, stdin, K4C_FILE_READER_MODE_BYTES, data, 0),
            K4C_STATUS_INVALID_ARGUMENT
        )
        != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(close_resets_reader_without_closing_stream) {
    char dir[256];
    if (make_temp_dir(dir, sizeof(dir)) != 0) {
        return 1;
    }

    char file_path[256];
    if (make_temp_file_path(file_path, sizeof(file_path), dir, "data.bin") != 0) {
        return cleanup_dir_and_return(dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_write_all(file_path, "abc", 3)) != 0) {
        return cleanup_file_dir_and_return(file_path, dir, 1);
    }

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        return cleanup_file_dir_and_return(file_path, dir, 1);
    }
    uint8_t data[64];
    k4c_file_reader reader;
    if (k4c_test_status_ok(
            k4c_file_reader_init(&reader, file, K4C_FILE_READER_MODE_BYTES, data, sizeof(data))
        )
        != 0) {
        return cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }

    if (k4c_test_status_ok(k4c_file_reader_close(&reader)) != 0) {
        return cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }
    if (k4c_test_equal(reader.file == NULL, true) != 0) {
        return cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }
    if (k4c_test_equal(reader.data == NULL, true) != 0) {
        return cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }
    if (k4c_test_equal(reader.len, 0) != 0) {
        return cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }
    if (k4c_test_equal(reader.buffer_capacity, 0) != 0) {
        return cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_reader_close(&reader)) != 0) {
        return cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }
    if (k4c_test_equal(fgetc(file), 'a') != 0) {
        return cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }

    return cleanup_stream_file_dir_and_return(file, file_path, dir, 0);
}

K4C_TEST(next_reads_byte_chunks) {
    char dir[256];
    if (make_temp_dir(dir, sizeof(dir)) != 0) {
        return 1;
    }

    char file_path[256];
    if (make_temp_file_path(file_path, sizeof(file_path), dir, "data.bin") != 0) {
        return cleanup_dir_and_return(dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_write_all(file_path, "abcdef", 6)) != 0) {
        return cleanup_file_dir_and_return(file_path, dir, 1);
    }

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        return cleanup_file_dir_and_return(file_path, dir, 1);
    }
    uint8_t data[2];
    k4c_file_reader reader;
    if (k4c_test_status_ok(
            k4c_file_reader_init(&reader, file, K4C_FILE_READER_MODE_BYTES, data, sizeof(data))
        )
        != 0) {
        return cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }

    k4c_buf_cursor cursor;
    if (k4c_test_status_ok(k4c_file_reader_next(&reader, &cursor)) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (cursor_equal(&cursor, "ab") != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_reader_next(&reader, &cursor)) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (cursor_equal(&cursor, "cd") != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_reader_next(&reader, &cursor)) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (cursor_equal(&cursor, "ef") != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(k4c_file_reader_next(&reader, &cursor), K4C_STATUS_EOF) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }

    return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 0);
}

K4C_TEST(next_reads_lines) {
    char dir[256];
    if (make_temp_dir(dir, sizeof(dir)) != 0) {
        return 1;
    }

    char file_path[256];
    if (make_temp_file_path(file_path, sizeof(file_path), dir, "data.txt") != 0) {
        return cleanup_dir_and_return(dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_write_all(file_path, "one\ntwo\nthree", 13)) != 0) {
        return cleanup_file_dir_and_return(file_path, dir, 1);
    }

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        return cleanup_file_dir_and_return(file_path, dir, 1);
    }
    uint8_t data[8];
    k4c_file_reader reader;
    if (k4c_test_status_ok(
            k4c_file_reader_init(&reader, file, K4C_FILE_READER_MODE_LINE, data, sizeof(data))
        )
        != 0) {
        return cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }

    k4c_buf_cursor cursor;
    if (k4c_test_status_ok(k4c_file_reader_next(&reader, &cursor)) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (cursor_equal(&cursor, "one\n") != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_reader_next(&reader, &cursor)) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (cursor_equal(&cursor, "two\n") != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_reader_next(&reader, &cursor)) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (cursor_equal(&cursor, "three") != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(k4c_file_reader_next(&reader, &cursor), K4C_STATUS_EOF) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }

    return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 0);
}

K4C_TEST(next_rejects_lines_over_buffer_capacity) {
    char dir[256];
    if (make_temp_dir(dir, sizeof(dir)) != 0) {
        return 1;
    }

    char file_path[256];
    if (make_temp_file_path(file_path, sizeof(file_path), dir, "data.txt") != 0) {
        return cleanup_dir_and_return(dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_write_all(file_path, "abcdef\n", 7)) != 0) {
        return cleanup_file_dir_and_return(file_path, dir, 1);
    }

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        return cleanup_file_dir_and_return(file_path, dir, 1);
    }
    uint8_t data[4];
    k4c_file_reader reader;
    if (k4c_test_status_ok(
            k4c_file_reader_init(&reader, file, K4C_FILE_READER_MODE_LINE, data, sizeof(data))
        )
        != 0) {
        return cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }

    k4c_buf_cursor cursor;
    if (k4c_test_equal(k4c_file_reader_next(&reader, &cursor), K4C_STATUS_OVERFLOW) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(reader.len, 4) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }

    return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 0);
}

K4C_TEST(next_accepts_final_line_at_buffer_capacity) {
    char dir[256];
    if (make_temp_dir(dir, sizeof(dir)) != 0) {
        return 1;
    }

    char file_path[256];
    if (make_temp_file_path(file_path, sizeof(file_path), dir, "data.txt") != 0) {
        return cleanup_dir_and_return(dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_write_all(file_path, "abcd", 4)) != 0) {
        return cleanup_file_dir_and_return(file_path, dir, 1);
    }

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        return cleanup_file_dir_and_return(file_path, dir, 1);
    }
    uint8_t data[4];
    k4c_file_reader reader;
    if (k4c_test_status_ok(
            k4c_file_reader_init(&reader, file, K4C_FILE_READER_MODE_LINE, data, sizeof(data))
        )
        != 0) {
        return cleanup_stream_file_dir_and_return(file, file_path, dir, 1);
    }

    k4c_buf_cursor cursor;
    if (k4c_test_status_ok(k4c_file_reader_next(&reader, &cursor)) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (cursor_equal(&cursor, "abcd") != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }
    if (k4c_test_equal(k4c_file_reader_next(&reader, &cursor), K4C_STATUS_EOF) != 0) {
        return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 1);
    }

    return cleanup_reader_stream_file_dir_and_return(&reader, file, file_path, dir, 0);
}

K4C_TEST_MAIN(
    K4C_TEST_CASE(init_initializes_reader),
    K4C_TEST_CASE(init_rejects_null_file),
    K4C_TEST_CASE(init_rejects_zero_buffer_capacity),
    K4C_TEST_CASE(close_resets_reader_without_closing_stream),
    K4C_TEST_CASE(next_reads_byte_chunks),
    K4C_TEST_CASE(next_reads_lines),
    K4C_TEST_CASE(next_rejects_lines_over_buffer_capacity),
    K4C_TEST_CASE(next_accepts_final_line_at_buffer_capacity)
)
