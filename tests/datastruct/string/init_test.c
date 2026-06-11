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

#include <stdio.h>
#include <string.h>

#include "ckit/datastruct/string.h"

int main(void) {
    int status = 0;
    ck_string empty = ck_string_init(NULL, NULL);
    if (empty == NULL || ck_string_len(empty) != 0 || strcmp(empty, "") != 0) {
        fprintf(stderr, "empty string should initialize to an empty C string\n");
        status = 1;
        goto cleanup_empty;
    }

cleanup_empty:
    ck_string_deinit(empty);
    if (status != 0) {
        return status;
    }

    ck_string value = ck_string_init("hello", NULL);
    if (value == NULL || ck_string_len(value) != 5 || strcmp(value, "hello") != 0) {
        fprintf(stderr, "string should initialize with contents\n");
        status = 1;
        goto cleanup_value;
    }

cleanup_value:
    ck_string_deinit(value);
    return status;
}
