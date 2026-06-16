# datastruct.string

## DESCRIPTION

The string module provides a growable NUL-terminated string. The public type is
`char *`, so it can be passed to C APIs that expect a normal string. Metadata is
stored before the visible character buffer.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### vs_string

```c
typedef char *vs_string;
```

## FUNCTIONS

### vs_string_create

```c
vs_string vs_string_create(const char *initial, vs_allocator *allocator);
```

- Parameters: `initial`, `allocator`
- Returns: created string.
- Notes: when `initial` is `NULL`, creates an empty string. The string stores
  `allocator` in its header and reuses it for growth and destroy. When
  `allocator` is `NULL`, uses the C library heap through
  `vs_malloc`/`vs_realloc`.

### vs_string_append

```c
void vs_string_append(vs_string *string, const char *suffix);
```

- Parameters: `string`, `suffix`
- Returns: none.
- Notes: may reallocate and update `*string`.

### vs_string_prepend

```c
void vs_string_prepend(vs_string *string, const char *prefix);
```

- Parameters: `string`, `prefix`
- Returns: none.
- Notes: may reallocate and update `*string`.

### vs_string_contains

```c
bool vs_string_contains(const vs_string string, const char *needle);
```

- Parameters: `string`, `needle`
- Returns: `true` when `needle` appears in `string`; otherwise `false`.
- Notes: an empty `needle` matches.

### vs_string_starts_with

```c
bool vs_string_starts_with(const vs_string string, const char *prefix);
```

- Parameters: `string`, `prefix`
- Returns: `true` when `string` begins with `prefix`; otherwise `false`.
- Notes: an empty `prefix` matches.

### vs_string_ends_with

```c
bool vs_string_ends_with(const vs_string string, const char *suffix);
```

- Parameters: `string`, `suffix`
- Returns: `true` when `string` ends with `suffix`; otherwise `false`.
- Notes: an empty `suffix` matches.

### vs_string_clear

```c
void vs_string_clear(vs_string string);
```

- Parameters: `string`
- Returns: none.
- Notes: resets length to zero without releasing storage.

### vs_string_len

```c
size_t vs_string_len(const vs_string string);
```

- Parameters: `string`
- Returns: number of bytes before the terminating NUL.

### vs_string_destroy

```c
void vs_string_destroy(vs_string string);
```

- Parameters: `string`
- Returns: none.
- Notes: releases the string allocation. Do not use `string` after this call.

## EXAMPLE

```c
#include <vstd/datastruct/string.h>
#include <string.h>

int main(void) {
    int status = 0;
    vs_string value = vs_string_create("hello", NULL);

    vs_string_append(&value, " world");
    vs_string_prepend(&value, "say ");
    if (strcmp(value, "say hello world") != 0) {
        status = 1;
        goto cleanup;
    }

cleanup:
    vs_string_destroy(value);
    return status;
}
```
