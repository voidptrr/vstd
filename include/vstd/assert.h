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

#ifndef VSTD_ASSERT_H
#define VSTD_ASSERT_H

#include "vstd/testing.h"

_Noreturn void vs_panic(const char *message);

#define VSTD_ASSERT(cond, message) \
    do { \
        if (!(cond)) { \
            vs_panic(message); \
        } \
    } while (0)

#define VS_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            return vs_test_fail(__FILE__, __LINE__, #condition, NULL); \
        } \
    } while (0)

#define VS_ASSERT_MSG(condition, message) \
    do { \
        if (!(condition)) { \
            return vs_test_fail(__FILE__, __LINE__, #condition, message); \
        } \
    } while (0)

#define VS_ASSERT_EQ(actual, expected) \
    do { \
        if ((actual) != (expected)) { \
            return vs_test_fail_eq(__FILE__, __LINE__, #actual, #expected); \
        } \
    } while (0)

#define VS_ASSERT_PTR_NULL(ptr) \
    do { \
        const void *vs_test_ptr = (ptr); \
        if (vs_test_ptr != NULL) { \
            return vs_test_fail_ptr_null(__FILE__, __LINE__, #ptr, vs_test_ptr); \
        } \
    } while (0)

#define VS_ASSERT_PTR_NOT_NULL(ptr) \
    do { \
        const void *vs_test_ptr = (ptr); \
        if (vs_test_ptr == NULL) { \
            return vs_test_fail(__FILE__, __LINE__, #ptr " != NULL", NULL); \
        } \
    } while (0)

#define VS_ASSERT_PTR_EQ(actual, expected) \
    do { \
        const void *vs_test_actual = (actual); \
        const void *vs_test_expected = (expected); \
        if (vs_test_actual != vs_test_expected) { \
            return vs_test_fail_ptr_eq( \
                __FILE__, \
                __LINE__, \
                #actual, \
                #expected, \
                vs_test_actual, \
                vs_test_expected \
            ); \
        } \
    } while (0)

#define VS_ASSERT_PTR_NE(actual, expected) \
    do { \
        const void *vs_test_actual = (actual); \
        const void *vs_test_expected = (expected); \
        if (vs_test_actual == vs_test_expected) { \
            return vs_test_fail(__FILE__, __LINE__, #actual " != " #expected, NULL); \
        } \
    } while (0)

#define VS_ASSERT_STR_EQ(actual, expected) \
    do { \
        const char *vs_test_actual = (actual); \
        const char *vs_test_expected = (expected); \
        if (!vs_test_str_eq(vs_test_actual, vs_test_expected)) { \
            return vs_test_fail_str_eq( \
                __FILE__, \
                __LINE__, \
                #actual, \
                #expected, \
                vs_test_actual, \
                vs_test_expected \
            ); \
        } \
    } while (0)

#endif
