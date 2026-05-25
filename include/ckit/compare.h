#ifndef CK_COMPARE_H
#define CK_COMPARE_H

#include <stdbool.h>
#include <stddef.h>

/* Shared callback shapes used by generic containers. */
typedef bool (*ck_eq_fn)(const void *lhs, const void *rhs, size_t size);
typedef int (*ck_cmp_fn)(const void *lhs, const void *rhs);

/*
 * Shared equality helpers for key matching (hashmap key_eq callbacks).
 * ck_eq_bytes honors size. ck_eq_cstr compares inline null-terminated
 * character data. ck_eq_cstr_ptr compares stored const char * values.
 * Scalar helpers ignore size. Pointer-string equality requires a matching hash
 * over string contents when used with hash-based containers.
 */
bool ck_eq_bytes(const void *lhs, const void *rhs, size_t size);
bool ck_eq_cstr(const void *lhs, const void *rhs, size_t size);
bool ck_eq_cstr_ptr(const void *lhs, const void *rhs, size_t size);
bool ck_eq_i32(const void *lhs, const void *rhs, size_t size);
bool ck_eq_i64(const void *lhs, const void *rhs, size_t size);
bool ck_eq_u32(const void *lhs, const void *rhs, size_t size);
bool ck_eq_u64(const void *lhs, const void *rhs, size_t size);
bool ck_eq_size(const void *lhs, const void *rhs, size_t size);

/*
 * Shared ordering helpers for container callbacks that use int-style
 * comparators (for example binary heap and future ordered utilities).
 */
int ck_cmp_i32(const void *lhs, const void *rhs);
int ck_cmp_i64(const void *lhs, const void *rhs);
int ck_cmp_u32(const void *lhs, const void *rhs);
int ck_cmp_u64(const void *lhs, const void *rhs);
int ck_cmp_size(const void *lhs, const void *rhs);

#endif
