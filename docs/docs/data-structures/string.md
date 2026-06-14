# datastruct.string

## DESCRIPTION

The string module provides a growable NUL-terminated string. The public type is
`char *`, so it can be passed to C APIs that expect a normal string. Metadata is
stored before the visible character buffer.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### ck_string

```c
typedef char *ck_string;
```

## FUNCTIONS

### ck_string_create

```c
ck_string ck_string_create(const char *initial, ck_allocator *allocator);
```

- Parameters: `initial`, `allocator`
- Returns: created string.
- Notes: when `initial` is `NULL`, creates an empty string. The string stores
  `allocator` in its header and reuses it for growth and destroy. When
  `allocator` is `NULL`, uses the C library heap through
  `ck_malloc`/`ck_realloc`.

### ck_string_append

```c
void ck_string_append(ck_string *string, const char *suffix);
```

- Parameters: `string`, `suffix`
- Returns: none.
- Notes: may reallocate and update `*string`.

### ck_string_prepend

```c
void ck_string_prepend(ck_string *string, const char *prefix);
```

- Parameters: `string`, `prefix`
- Returns: none.
- Notes: may reallocate and update `*string`.

### ck_string_contains

```c
bool ck_string_contains(const ck_string string, const char *needle);
```

- Parameters: `string`, `needle`
- Returns: `true` when `needle` appears in `string`; otherwise `false`.
- Notes: an empty `needle` matches.

### ck_string_starts_with

```c
bool ck_string_starts_with(const ck_string string, const char *prefix);
```

- Parameters: `string`, `prefix`
- Returns: `true` when `string` begins with `prefix`; otherwise `false`.
- Notes: an empty `prefix` matches.

### ck_string_ends_with

```c
bool ck_string_ends_with(const ck_string string, const char *suffix);
```

- Parameters: `string`, `suffix`
- Returns: `true` when `string` ends with `suffix`; otherwise `false`.
- Notes: an empty `suffix` matches.

### ck_string_clear

```c
void ck_string_clear(ck_string string);
```

- Parameters: `string`
- Returns: none.
- Notes: resets length to zero without releasing storage.

### ck_string_len

```c
size_t ck_string_len(const ck_string string);
```

- Parameters: `string`
- Returns: number of bytes before the terminating NUL.

### ck_string_destroy

```c
void ck_string_destroy(ck_string string);
```

- Parameters: `string`
- Returns: none.
- Notes: releases the string allocation. Do not use `string` after this call.

## EXAMPLE

```c
#include <ckit/datastruct/string.h>
#include <string.h>

int main(void) {
    int status = 0;
    ck_string value = ck_string_create("hello", NULL);

    ck_string_append(&value, " world");
    ck_string_prepend(&value, "say ");
    if (strcmp(value, "say hello world") != 0) {
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_string_destroy(value);
    return status;
}
```
