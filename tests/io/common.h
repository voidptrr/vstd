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

#ifndef K4C_IO_TEST_COMMON_H
#define K4C_IO_TEST_COMMON_H

#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

static inline int k4c_io_test_make_temp_dir(char *out, size_t out_size, const char *prefix) {
    static unsigned int counter;

    int written = snprintf(out, out_size, "/tmp/%s-%ld-%u", prefix, (long)getpid(), counter++);
    if (written < 0 || (size_t)written >= out_size) {
        return 1;
    }
    return mkdir(out, 0700) != 0 ? 1 : 0;
}

static inline int k4c_io_test_make_temp_file_path(
    char *out,
    size_t out_size,
    const char *dir,
    const char *name
) {
    int written = snprintf(out, out_size, "%s/%s", dir, name);
    if (written < 0 || (size_t)written >= out_size) {
        return 1;
    }
    return 0;
}

static inline int k4c_io_test_cleanup_dir_and_return(const char *dir, int result) {
    if (rmdir(dir) != 0) {
        return 1;
    }
    return result;
}

static inline int k4c_io_test_cleanup_file_dir_and_return(
    const char *file_path,
    const char *dir,
    int result
) {
    int cleanup_result = 0;

    if (unlink(file_path) != 0) {
        cleanup_result = 1;
    }
    if (rmdir(dir) != 0) {
        cleanup_result = 1;
    }

    return cleanup_result != 0 ? 1 : result;
}

static inline int k4c_io_test_cleanup_stream_file_dir_and_return(
    FILE *file,
    const char *file_path,
    const char *dir,
    int result
) {
    int cleanup_result = 0;

    if (file != NULL && fclose(file) != 0) {
        cleanup_result = 1;
    }
    if (k4c_io_test_cleanup_file_dir_and_return(file_path, dir, result) != result) {
        cleanup_result = 1;
    }

    return cleanup_result != 0 ? 1 : result;
}

#endif
