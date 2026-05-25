#include <stdint.h>
#include <string.h>

#include "ckit/compare.h"

bool ck_eq_bytes(const void *lhs, const void *rhs, size_t size) {
    return memcmp(lhs, rhs, size) == 0;
}

bool ck_eq_cstr(const void *lhs, const void *rhs, size_t size) {
    (void)size;
    return strcmp((const char *)lhs, (const char *)rhs) == 0;
}

bool ck_eq_cstr_ptr(const void *lhs, const void *rhs, size_t size) {
    (void)size;
    const char *a = *(const char *const *)lhs;
    const char *b = *(const char *const *)rhs;
    return strcmp(a, b) == 0;
}

bool ck_eq_i32(const void *lhs, const void *rhs, size_t size) {
    (void)size;
    return (*(const int32_t *)lhs) == (*(const int32_t *)rhs);
}

bool ck_eq_i64(const void *lhs, const void *rhs, size_t size) {
    (void)size;
    return (*(const int64_t *)lhs) == (*(const int64_t *)rhs);
}

bool ck_eq_u32(const void *lhs, const void *rhs, size_t size) {
    (void)size;
    return (*(const uint32_t *)lhs) == (*(const uint32_t *)rhs);
}

bool ck_eq_u64(const void *lhs, const void *rhs, size_t size) {
    (void)size;
    return (*(const uint64_t *)lhs) == (*(const uint64_t *)rhs);
}

bool ck_eq_size(const void *lhs, const void *rhs, size_t size) {
    (void)size;
    return (*(const size_t *)lhs) == (*(const size_t *)rhs);
}

int ck_cmp_i32(const void *lhs, const void *rhs) {
    int32_t a = *(const int32_t *)lhs;
    int32_t b = *(const int32_t *)rhs;
    return (a > b) - (a < b);
}

int ck_cmp_i64(const void *lhs, const void *rhs) {
    int64_t a = *(const int64_t *)lhs;
    int64_t b = *(const int64_t *)rhs;
    return (a > b) - (a < b);
}

int ck_cmp_u32(const void *lhs, const void *rhs) {
    uint32_t a = *(const uint32_t *)lhs;
    uint32_t b = *(const uint32_t *)rhs;
    return (a > b) - (a < b);
}

int ck_cmp_u64(const void *lhs, const void *rhs) {
    uint64_t a = *(const uint64_t *)lhs;
    uint64_t b = *(const uint64_t *)rhs;
    return (a > b) - (a < b);
}

int ck_cmp_size(const void *lhs, const void *rhs) {
    size_t a = *(const size_t *)lhs;
    size_t b = *(const size_t *)rhs;
    return (a > b) - (a < b);
}
