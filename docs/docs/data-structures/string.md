# datastruct.string

## DESCRIPTION

The string module provides a growable NUL-terminated string. The public type is
`char *`, so it can be passed to C APIs that expect a normal string. Metadata is
stored before the visible character buffer.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### ckit_string

```c
typedef char *ckit_string;
```

## FUNCTIONS

### ckit_string_init

```c
ckit_string ckit_string_init(const char *initial, ckit_allocator *allocator);
```

- Parameters: `initial`, `allocator`
- Returns: initialized string.
- Notes: when `initial` is `NULL`, creates an empty string. When `allocator` is `NULL`, uses the C library heap through `ckit_malloc`.

### ckit_string_append

```c
void ckit_string_append(ckit_string *string, const char *suffix);
```

- Parameters: `string`, `suffix`
- Returns: none.
- Notes: may reallocate and update `*string`.

### ckit_string_prepend

```c
void ckit_string_prepend(ckit_string *string, const char *prefix);
```

- Parameters: `string`, `prefix`
- Returns: none.
- Notes: may reallocate and update `*string`.

### ckit_string_contains

```c
bool ckit_string_contains(const ckit_string string, const char *needle);
```

- Parameters: `string`, `needle`
- Returns: `true` when `needle` appears in `string`; otherwise `false`.
- Notes: an empty `needle` matches.

### ckit_string_starts_with

```c
bool ckit_string_starts_with(const ckit_string string, const char *prefix);
```

- Parameters: `string`, `prefix`
- Returns: `true` when `string` begins with `prefix`; otherwise `false`.
- Notes: an empty `prefix` matches.

### ckit_string_ends_with

```c
bool ckit_string_ends_with(const ckit_string string, const char *suffix);
```

- Parameters: `string`, `suffix`
- Returns: `true` when `string` ends with `suffix`; otherwise `false`.
- Notes: an empty `suffix` matches.

### ckit_string_clear

```c
void ckit_string_clear(ckit_string string);
```

- Parameters: `string`
- Returns: none.
- Notes: resets length to zero without releasing storage.

### ckit_string_len

```c
size_t ckit_string_len(const ckit_string string);
```

- Parameters: `string`
- Returns: number of bytes before the terminating NUL.

### ckit_string_deinit

```c
void ckit_string_deinit(ckit_string string);
```

- Parameters: `string`
- Returns: none.
- Notes: releases the string allocation. Do not use `string` after this call.

## EXAMPLE

```c
#include <ckit/datastruct/string.h>
#include <string.h>

int main(void) {
    ckit_string value = ckit_string_init("hello", NULL);

    ckit_string_append(&value, " world");
    ckit_string_prepend(&value, "say ");
    if (strcmp(value, "say hello world") != 0) {
        ckit_string_deinit(value);
        return 1;
    }

    ckit_string_deinit(value);
    return 0;
}
```
