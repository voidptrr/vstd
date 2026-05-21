#ifndef CKIT_DATASTRUCT_STRING_H
#define CKIT_DATASTRUCT_STRING_H

#include <stddef.h>

#include "ckit/memory/allocators/allocator.h"

typedef char *ckit_string;

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

/* Initialize a string from initial, or an empty string when initial is NULL. */
ckit_string ckit_string_init(const char *initial, ckit_allocator *allocator);

/* Append suffix to string, growing storage as needed. */
void ckit_string_append(ckit_string *string, const char *suffix);

/* Release owned storage. */
void ckit_string_free(ckit_string string);

/* Return the number of bytes before the terminating NUL. */
size_t ckit_string_len(const ckit_string string);

#endif
