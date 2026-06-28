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

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "k4c/assert.h"
#include "k4c/error.h"
#include "k4c/io/file.h"
#include "k4c/memory/allocator.h"

static bool k4c_file_errno_is_not_found(void) {
    return errno == ENOENT || errno == ENOTDIR;
}

static k4c_status k4c_file_stat(const char *path, struct stat *out) {
    K4C_ASSERT(path != NULL, "fatal: k4c_file_stat invalid arguments");
    K4C_ASSERT(out != NULL, "fatal: k4c_file_stat invalid arguments");

    if (stat(path, out) != 0) {
        if (k4c_file_errno_is_not_found()) {
            return K4C_STATUS_NOT_FOUND;
        }
        return K4C_STATUS_IO;
    }
    return K4C_STATUS_OK;
}

k4c_status k4c_file_is_dir(const char *path, bool *out) {
    K4C_ASSERT(path != NULL, "fatal: k4c_file_is_dir invalid arguments");
    K4C_ASSERT(out != NULL, "fatal: k4c_file_is_dir invalid arguments");

    *out = false;

    struct stat st;
    K4C_RETURN_IF_ERROR(k4c_file_stat(path, &st));

    *out = S_ISDIR(st.st_mode);
    return K4C_STATUS_OK;
}

k4c_status k4c_file_size(const char *path, size_t *out) {
    K4C_ASSERT(path != NULL, "fatal: k4c_file_size invalid arguments");
    K4C_ASSERT(out != NULL, "fatal: k4c_file_size invalid arguments");

    *out = 0;

    struct stat st;
    K4C_RETURN_IF_ERROR(k4c_file_stat(path, &st));
    if (!S_ISREG(st.st_mode)) {
        return K4C_STATUS_INVALID_ARGUMENT;
    }
    if (st.st_size < 0 || (uintmax_t)st.st_size > SIZE_MAX) {
        return K4C_STATUS_OVERFLOW;
    }

    *out = (size_t)st.st_size;
    return K4C_STATUS_OK;
}

k4c_status k4c_file_read_all(
    const char *path,
    k4c_allocator *k4c_allocator,
    uint8_t **out_data,
    size_t *out_len
) {
    K4C_ASSERT(path != NULL, "fatal: k4c_file_read_all invalid arguments");
    K4C_ASSERT(out_data != NULL, "fatal: k4c_file_read_all invalid arguments");
    K4C_ASSERT(out_len != NULL, "fatal: k4c_file_read_all invalid arguments");

    *out_data = NULL;
    *out_len = 0;

    size_t len = 0;
    K4C_RETURN_IF_ERROR(k4c_file_size(path, &len));
    if (len == SIZE_MAX) {
        return K4C_STATUS_OVERFLOW;
    }

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        if (k4c_file_errno_is_not_found()) {
            return K4C_STATUS_NOT_FOUND;
        }
        return K4C_STATUS_IO;
    }

    size_t alloc_len = len + 1;

    uint8_t *data = NULL;
    k4c_status st = k4c_alloc(k4c_allocator, alloc_len, (void **)&data);
    if (st != K4C_STATUS_OK) {
        (void)fclose(file);
        return st;
    }
    memset(data, 0, alloc_len);

    size_t read_len = fread(data, 1, len, file);
    if (read_len != len || ferror(file)) {
        k4c_dealloc(k4c_allocator, data);
        (void)fclose(file);
        return K4C_STATUS_IO;
    }

    if (fclose(file) != 0) {
        k4c_dealloc(k4c_allocator, data);
        return K4C_STATUS_IO;
    }

    *out_data = data;
    *out_len = len;
    return K4C_STATUS_OK;
}

k4c_status k4c_file_write_all(const char *path, const void *data, size_t len) {
    K4C_ASSERT(path != NULL, "fatal: k4c_file_write_all invalid arguments");
    K4C_ASSERT(data != NULL || len == 0, "fatal: k4c_file_write_all invalid arguments");

    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        return K4C_STATUS_IO;
    }

    if (len > 0 && fwrite(data, 1, len, file) != len) {
        (void)fclose(file);
        return K4C_STATUS_IO;
    }

    if (fclose(file) != 0) {
        return K4C_STATUS_IO;
    }
    return K4C_STATUS_OK;
}
