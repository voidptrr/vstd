#ifndef CKIT_DATASTRUCT_STRING_H
#define CKIT_DATASTRUCT_STRING_H

#include <stdbool.h>
#include <stddef.h>

#include "ckit/memory/allocators/allocator.h"

/*
 * Opaque growable string.
 *
 * Public view:
 *
 *   ckit_string
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
 *                                            ckit_string
 */
typedef char *ckit_string;

/* Initialize a string from initial, or an empty string when initial is NULL. */
ckit_string ckit_string_init(const char *initial, ckit_allocator *allocator);

/* Append suffix to string, growing storage as needed. */
void ckit_string_append(ckit_string *string, const char *suffix);

/* Prepend prefix to string, growing storage as needed. */
void ckit_string_prepend(ckit_string *string, const char *prefix);

/* Return whether string contains needle. */
bool ckit_string_contains(const ckit_string string, const char *needle);

/* Return whether string begins with prefix. */
bool ckit_string_starts_with(const ckit_string string, const char *prefix);

/* Return whether string ends with suffix. */
bool ckit_string_ends_with(const ckit_string string, const char *suffix);

/* Reset string to empty without releasing storage. */
void ckit_string_clear(ckit_string string);

/* Return the number of bytes before the terminating NUL. */
size_t ckit_string_len(const ckit_string string);

/* Deinitialize and release owned storage. */
void ckit_string_deinit(ckit_string string);

#endif
