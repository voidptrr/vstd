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

#include "ckit/datastruct/string.h"

int main(void) {
    int status = 0;
    ck_string value = ck_string_init("hello world", NULL);

    if (!ck_string_contains(value, "lo wo")) {
        fprintf(stderr, "contains should find substring\n");
        status = 1;
        goto cleanup;
    }

    if (ck_string_contains(value, "missing")) {
        fprintf(stderr, "contains should reject missing substring\n");
        status = 1;
        goto cleanup;
    }

    if (!ck_string_starts_with(value, "hello") || ck_string_starts_with(value, "world")) {
        fprintf(stderr, "starts_with should check prefix\n");
        status = 1;
        goto cleanup;
    }

    if (!ck_string_ends_with(value, "world") || ck_string_ends_with(value, "hello")) {
        fprintf(stderr, "ends_with should check suffix\n");
        status = 1;
        goto cleanup;
    }

    if (!ck_string_contains(value, "") || !ck_string_starts_with(value, "") ||
        !ck_string_ends_with(value, "")) {
        fprintf(stderr, "empty needle/prefix/suffix should match\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_string_deinit(value);
    return status;
}
