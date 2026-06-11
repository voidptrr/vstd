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
#include <stdio.h>

#include "ckit/compare.h"
#include "ckit/datastruct/hashset.h"

int main(void) {
    int status = 0;
    ck_hashset *set;
    const ck_hashset *const_set;
    uint64_t value = 42;
    uint64_t missing = 7;
    uint64_t *found;
    const uint64_t *const_found;

    set = ck_hashset_init(sizeof(uint64_t), ck_eq_u64, NULL);

    if (ck_hashset_get(set, &value) != NULL) {
        fprintf(stderr, "missing element should return NULL\n");
        status = 1;
        goto cleanup;
    }

    ck_hashset_insert(set, &value);

    found = (uint64_t *)ck_hashset_get(set, &value);
    if (found == NULL || *found != value) {
        fprintf(stderr, "get should return inserted element\n");
        status = 1;
        goto cleanup;
    }

    const_set = set;
    const_found = (const uint64_t *)ck_hashset_get_const(const_set, &value);
    if (const_found == NULL || *const_found != value) {
        fprintf(stderr, "get_const should return inserted element\n");
        status = 1;
        goto cleanup;
    }

    if (ck_hashset_get(set, &missing) != NULL) {
        fprintf(stderr, "get should return NULL for missing element\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_hashset_deinit(set);
    return status;
}
