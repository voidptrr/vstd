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
#include <stdint.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashset.h"

int main(void) {
    int status = 0;
    ck_hashset *set;

    set = ck_hashset_init(sizeof(uint64_t), ck_eq_u64, NULL);

    for (uint64_t i = 0; i < 256; i++) {
        ck_hashset_insert(set, &i);
    }

    uint64_t removed = 42;
    ck_hashset_remove(set, &removed);
    if (ck_hashset_contains(set, &removed) || ck_hashset_size(set) != 255) {
        fprintf(stderr, "hashset remove should delete existing value\n");
        status = 1;
        goto cleanup;
    }

    ck_hashset_remove(set, &removed);
    if (ck_hashset_size(set) != 255) {
        fprintf(stderr, "hashset remove should ignore missing value\n");
        status = 1;
        goto cleanup;
    }

    uint64_t first = 0;
    uint64_t last = 255;
    ck_hashset_remove(set, &first);
    ck_hashset_remove(set, &last);
    if (ck_hashset_contains(set, &first) || ck_hashset_contains(set, &last) ||
        ck_hashset_size(set) != 253) {
        fprintf(stderr, "hashset remove should handle bucket head and chain nodes\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_hashset_deinit(set);
    return status;
}
