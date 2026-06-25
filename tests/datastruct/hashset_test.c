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

#include "vstd/datastruct/hashset.h"
#include "vstd/datastruct/iterator.h"
#include "vstd/memory/test_allocator.h"
#include "vstd/testing.h"

static size_t custom_eq_calls;

static bool custom_u64_eq(const void *lhs, const void *rhs) {
    custom_eq_calls += 1;
    return *(const uint64_t *)lhs == *(const uint64_t *)rhs;
}

VS_TEST(allocator) {
    vs_test_allocator test_allocator;
    uint64_t value = 11;

    vs_test_allocator_init(&test_allocator);
    vs_hashset *set =
        vs_hashset_create(sizeof(uint64_t), NULL, vs_test_allocator_adapter(&test_allocator));
    vs_test_allocator_reset_counts(&test_allocator);

    vs_hashset_insert(set, &value);
    if (test_allocator.alloc_count != 1) {
        return 1;
    }

    vs_hashset_destroy(set);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(contains) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashset *set;

    set = vs_hashset_create(sizeof(uint64_t), NULL, vs_test_allocator_adapter(&test_allocator));

    uint64_t present = 42;
    uint64_t missing = 7;
    if (vs_test_equal(!vs_hashset_contains(set, &present), true) != 0) {
        return 1;
    }

    vs_hashset_insert(set, &present);
    if (vs_test_equal(vs_hashset_contains(set, &present), true) != 0) {
        return 1;
    }

    if (vs_test_equal(!vs_hashset_contains(set, &missing), true) != 0) {
        return 1;
    }

    for (uint64_t i = 0; i < 256; i++) {
        vs_hashset_insert(set, &i);
    }

    if (vs_test_equal(vs_hashset_contains(set, &missing), true) != 0) {
        return 1;
    }

    vs_hashset_destroy(set);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(get) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashset *set;
    const vs_hashset *const_set;
    uint64_t value = 42;
    uint64_t missing = 7;
    uint64_t *found;
    const uint64_t *const_found;

    set = vs_hashset_create(sizeof(uint64_t), NULL, vs_test_allocator_adapter(&test_allocator));

    if (vs_test_null(vs_hashset_get(set, &value)) != 0) {
        return 1;
    }

    vs_hashset_insert(set, &value);

    found = (uint64_t *)vs_hashset_get(set, &value);
    if (vs_test_not_null(found) != 0) {
        return 1;
    }
    if (vs_test_equal(*found, value) != 0) {
        return 1;
    }

    const_set = set;
    const_found = (const uint64_t *)vs_hashset_get_const(const_set, &value);
    if (vs_test_not_null(const_found) != 0) {
        return 1;
    }
    if (vs_test_equal(*const_found, value) != 0) {
        return 1;
    }

    if (vs_test_null(vs_hashset_get(set, &missing)) != 0) {
        return 1;
    }

    vs_hashset_destroy(set);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(init) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashset *set;

    set = vs_hashset_create(sizeof(uint64_t), NULL, vs_test_allocator_adapter(&test_allocator));
    if (vs_hashset_size(set) != 0) {
        return 1;
    }

    vs_hashset_destroy(set);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(default_byte_equality) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashset *set;
    uint64_t value = 42;
    uint64_t same_value = 42;

    set = vs_hashset_create(sizeof(uint64_t), NULL, vs_test_allocator_adapter(&test_allocator));

    vs_hashset_insert(set, &value);
    vs_hashset_insert(set, &same_value);
    if (vs_test_equal(vs_hashset_contains(set, &same_value), true) != 0) {
        return 1;
    }
    if (vs_hashset_size(set) != 1) {
        return 1;
    }

    vs_hashset_destroy(set);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(custom_equality) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashset *set;
    uint64_t value = 42;
    uint64_t same_value = 42;

    custom_eq_calls = 0;
    set = vs_hashset_create(
        sizeof(uint64_t),
        custom_u64_eq,
        vs_test_allocator_adapter(&test_allocator)
    );

    vs_hashset_insert(set, &value);
    vs_hashset_insert(set, &same_value);
    if (vs_test_equal(vs_hashset_contains(set, &same_value), true) != 0) {
        return 1;
    }
    if (vs_hashset_size(set) != 1) {
        return 1;
    }
    if (vs_test_equal(custom_eq_calls > 0, true) != 0) {
        return 1;
    }

    vs_hashset_destroy(set);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(insert) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashset *set;

    set = vs_hashset_create(sizeof(uint64_t), NULL, vs_test_allocator_adapter(&test_allocator));

    uint64_t first = 42;
    vs_hashset_insert(set, &first);
    vs_hashset_insert(set, &first);
    if (vs_hashset_size(set) != 1) {
        return 1;
    }

    for (uint64_t i = 0; i < 256; i++) {
        vs_hashset_insert(set, &i);
    }

    if (vs_hashset_size(set) != 256) {
        return 1;
    }

    vs_hashset_destroy(set);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(reserve) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashset *set;

    set = vs_hashset_create(sizeof(uint64_t), NULL, vs_test_allocator_adapter(&test_allocator));
    vs_hashset_reserve(set, 512);

    for (uint64_t i = 0; i < 512; i++) {
        vs_hashset_insert(set, &i);
    }

    for (uint64_t i = 0; i < 512; i++) {
        if (vs_test_equal(vs_hashset_contains(set, &i), true) != 0) {
            return 1;
        }
    }

    vs_hashset_destroy(set);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(remove) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashset *set;

    set = vs_hashset_create(sizeof(uint64_t), NULL, vs_test_allocator_adapter(&test_allocator));

    for (uint64_t i = 0; i < 256; i++) {
        vs_hashset_insert(set, &i);
    }

    uint64_t removed = 42;
    vs_hashset_remove(set, &removed);
    if (vs_test_equal(!vs_hashset_contains(set, &removed), true) != 0) {
        return 1;
    }
    if (vs_hashset_size(set) != 255) {
        return 1;
    }

    vs_hashset_remove(set, &removed);
    if (vs_hashset_size(set) != 255) {
        return 1;
    }

    uint64_t first = 0;
    uint64_t last = 255;
    vs_hashset_remove(set, &first);
    vs_hashset_remove(set, &last);
    if (vs_test_equal(!vs_hashset_contains(set, &first), true) != 0) {
        return 1;
    }
    if (vs_test_equal(!vs_hashset_contains(set, &last), true) != 0) {
        return 1;
    }
    if (vs_hashset_size(set) != 253) {
        return 1;
    }

    vs_hashset_destroy(set);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(foreach_macro_walks_elements) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashset *set;
    const uint64_t *elem;
    uint64_t sum = 0;
    size_t count = 0;

    set = vs_hashset_create(sizeof(uint64_t), NULL, vs_test_allocator_adapter(&test_allocator));
    for (uint64_t i = 1; i <= 4; i++) {
        vs_hashset_insert(set, &i);
    }

    VS_HASHSET_FOR_EACH(uint64_t, elem, set) {
        sum += *elem;
        count += 1;
    }

    if (count != 4) {
        return 1;
    }
    if (sum != 10) {
        return 1;
    }

    vs_hashset_destroy(set);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST(iterator_walks_elements) {
    vs_test_allocator test_allocator;
    vs_test_allocator_init(&test_allocator);
    vs_hashset *set;
    vs_iterator iter;
    const uint64_t *elem;
    uint64_t sum = 0;
    size_t count = 0;

    set = vs_hashset_create(sizeof(uint64_t), NULL, vs_test_allocator_adapter(&test_allocator));
    for (uint64_t i = 1; i <= 4; i++) {
        vs_hashset_insert(set, &i);
    }

    iter = vs_hashset_get_iterator(set);
    while ((elem = (const uint64_t *)vs_iterator_next(&iter)) != NULL) {
        sum += *elem;
        count += 1;
    }
    if (count != 4) {
        return 1;
    }
    if (sum != 10) {
        return 1;
    }

    vs_hashset_destroy(set);
    if (vs_test_equal(vs_test_allocator_is_clean(&test_allocator), true) != 0) {
        return 1;
    }
    return 0;
}

VS_TEST_MAIN(
    VS_TEST_CASE(allocator),
    VS_TEST_CASE(contains),
    VS_TEST_CASE(get),
    VS_TEST_CASE(init),
    VS_TEST_CASE(default_byte_equality),
    VS_TEST_CASE(custom_equality),
    VS_TEST_CASE(insert),
    VS_TEST_CASE(reserve),
    VS_TEST_CASE(remove),
    VS_TEST_CASE(foreach_macro_walks_elements),
    VS_TEST_CASE(iterator_walks_elements)
)
