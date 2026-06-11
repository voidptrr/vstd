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
#include "ckit/datastruct/hashmap.h"

int main(void) {
    int status = 0;
    ck_hashmap *map;
    uint64_t key = 7;
    uint64_t value = 11;
    uint64_t value2 = 99;
    const uint64_t *out;
    const ck_hashmap *const_map;

    map = ck_hashmap_init(sizeof(uint64_t), sizeof(uint64_t), ck_eq_u64, NULL);

    if (ck_hashmap_get(map, &key) != NULL) {
        fprintf(stderr, "missing key should return NULL\n");
        status = 1;
        goto cleanup;
    }

    ck_hashmap_put(map, &key, &value);
    out = (uint64_t *)ck_hashmap_get(map, &key);
    if (out == NULL || *out != value) {
        fprintf(stderr, "get should return inserted value\n");
        status = 1;
        goto cleanup;
    }

    const_map = map;
    out = (const uint64_t *)ck_hashmap_get_const(const_map, &key);
    if (out == NULL || *out != value) {
        fprintf(stderr, "get_const should return inserted value\n");
        status = 1;
        goto cleanup;
    }

    ck_hashmap_put(map, &key, &value2);
    out = (uint64_t *)ck_hashmap_get(map, &key);
    if (out == NULL || *out != value2 || ck_hashmap_size(map) != 1) {
        fprintf(stderr, "put should update existing key in place\n");
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_hashmap_deinit(map);
    return status;
}
