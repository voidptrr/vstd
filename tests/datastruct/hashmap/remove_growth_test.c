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

#include "ckit/testing.h"
#include "ckit/compare.h"
#include "ckit/datastruct/hashmap.h"

int main(void) {
    ck_hashmap *map;
    uint64_t key = 128;

    map = ck_hashmap_create(sizeof(uint64_t), sizeof(uint64_t), ck_eq_u64, NULL);

    for (uint64_t i = 0; i < 256; i++) {
        uint64_t value = i * 3;
        ck_hashmap_put(map, &i, &value);
    }

    CK_TEST_ASSERT_EQ(ck_hashmap_size(map), 256);

    ck_hashmap_remove(map, &key);
    CK_TEST_ASSERT_PTR_NULL(ck_hashmap_get(map, &key));

    ck_hashmap_destroy(map);
    return 0;
}
