#ifndef CK_DATASTRUCT_STRING_H
#define CK_DATASTRUCT_STRING_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/memory/allocators/allocator.h"

/*
 * Opaque growable string.
 *
 * Public view:
 *
 *   ck_string
 *        |
 *        v
 *      "hello\0"
 *
 * Internal storage keeps metadata before the visible character buffer:
 *
 *   +----------------------+----------------------+-------------+
 *   | length / capacity    | allocator metadata   | char buf[]  |
 *   +----------------------+----------------------+-------------+
 *                                                 ^
 *                                                 |
 *                                            ck_string
 */
typedef char *ck_string;

/* Initialize a string from initial, or an empty string when initial is NULL. */
ck_string ck_string_init(const char *initial, ck_allocator *allocator);

/* Append suffix to string, growing storage as needed. */
void ck_string_append(ck_string *string, const char *suffix);

/* Prepend prefix to string, growing storage as needed. */
void ck_string_prepend(ck_string *string, const char *prefix);

/* Return whether string contains needle. */
bool ck_string_contains(const ck_string string, const char *needle);

/* Return whether string begins with prefix. */
bool ck_string_starts_with(const ck_string string, const char *prefix);

/* Return whether string ends with suffix. */
bool ck_string_ends_with(const ck_string string, const char *suffix);

/* Reset string to empty without releasing storage. */
void ck_string_clear(ck_string string);

/* Return the number of bytes before the terminating NUL. */
size_t ck_string_len(const ck_string string);

/* Deinitialize and release owned storage. */
void ck_string_deinit(ck_string string);

#endif
