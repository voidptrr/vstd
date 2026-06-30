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
#include <string.h>

#include "common.h"
#include "k4c/allocators/allocator.h"
#include "k4c/allocators/test_allocator.h"
#include "k4c/error.h"
#include "k4c/io/file.h"
#include "k4c/testing.h"

K4C_TEST(is_dir_reports_directories_and_files) {
    char dir[256];
    if (k4c_io_test_make_temp_dir(dir, sizeof(dir), "k4c-file-test") != 0) {
        return 1;
    }

    char file_path[256];
    if (k4c_io_test_make_temp_file_path(file_path, sizeof(file_path), dir, "data.bin") != 0) {
        return k4c_io_test_cleanup_dir_and_return(dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_write_all(file_path, "abc", 3)) != 0) {
        return k4c_io_test_cleanup_dir_and_return(dir, 1);
    }

    bool is_dir = false;
    if (k4c_test_status_ok(k4c_file_is_dir(dir, &is_dir)) != 0) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }
    if (k4c_test_equal(is_dir, true) != 0) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }

    if (k4c_test_status_ok(k4c_file_is_dir(file_path, &is_dir)) != 0) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }
    if (k4c_test_equal(is_dir, false) != 0) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }

    return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 0);
}

K4C_TEST(missing_path_returns_not_found) {
    bool is_dir = true;
    size_t size = 99;

    if (k4c_test_equal(k4c_file_is_dir("/tmp/k4c-missing-file-path", &is_dir), K4C_STATUS_NOT_FOUND)
        != 0) {
        return 1;
    }
    if (k4c_test_equal(is_dir, false) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_file_size("/tmp/k4c-missing-file-path", &size), K4C_STATUS_NOT_FOUND)
        != 0) {
        return 1;
    }
    if (k4c_test_equal(size, 0) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST(file_size_reports_regular_file_size) {
    char dir[256];
    if (k4c_io_test_make_temp_dir(dir, sizeof(dir), "k4c-file-test") != 0) {
        return 1;
    }

    char file_path[256];
    if (k4c_io_test_make_temp_file_path(file_path, sizeof(file_path), dir, "data.bin") != 0) {
        return k4c_io_test_cleanup_dir_and_return(dir, 1);
    }
    if (k4c_test_status_ok(k4c_file_write_all(file_path, "hello", 5)) != 0) {
        return k4c_io_test_cleanup_dir_and_return(dir, 1);
    }

    size_t size = 0;
    if (k4c_test_status_ok(k4c_file_size(file_path, &size)) != 0) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }
    if (k4c_test_equal(size, 5) != 0) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }

    return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 0);
}

K4C_TEST(write_all_and_read_all_round_trip) {
    char dir[256];
    if (k4c_io_test_make_temp_dir(dir, sizeof(dir), "k4c-file-test") != 0) {
        return 1;
    }

    char file_path[256];
    if (k4c_io_test_make_temp_file_path(file_path, sizeof(file_path), dir, "data.bin") != 0) {
        return k4c_io_test_cleanup_dir_and_return(dir, 1);
    }

    const uint8_t expected[] = {0, 1, 2, 3, 255};
    if (k4c_test_status_ok(k4c_file_write_all(file_path, expected, sizeof(expected))) != 0) {
        return k4c_io_test_cleanup_dir_and_return(dir, 1);
    }

    k4c_test_allocator k4c_test_allocator;
    k4c_allocator allocator = k4c_test_allocator_init(&k4c_test_allocator);
    k4c_allocator *k4c_allocator = &allocator;
    uint8_t *data = NULL;
    size_t len = 0;
    if (k4c_test_status_ok(k4c_file_read_all(file_path, k4c_allocator, &data, &len)) != 0) {
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }
    if (k4c_test_equal(len, sizeof(expected)) != 0) {
        k4c_dealloc(k4c_allocator, data);
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }
    if (memcmp(data, expected, sizeof(expected)) != 0) {
        k4c_dealloc(k4c_allocator, data);
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }
    if (k4c_test_equal(data[len], 0) != 0) {
        k4c_dealloc(k4c_allocator, data);
        return k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 1);
    }

    k4c_dealloc(k4c_allocator, data);
    if (k4c_io_test_cleanup_file_dir_and_return(file_path, dir, 0) != 0) {
        return 1;
    }
    if (k4c_test_equal(k4c_test_allocator_is_clean(&k4c_test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

K4C_TEST_MAIN(
    K4C_TEST_CASE(is_dir_reports_directories_and_files),
    K4C_TEST_CASE(missing_path_returns_not_found),
    K4C_TEST_CASE(file_size_reports_regular_file_size),
    K4C_TEST_CASE(write_all_and_read_all_round_trip)
)
