# ds.string

## DESCRIPTION

The string module provides a growable NUL-terminated string. The public type is
`char *`, so it can be passed to C APIs that expect a normal string. Metadata is
stored before the visible character buffer.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### string

```c
typedef char *string;
```

## FUNCTIONS

### string_create

```c
status string_create(const char *initial, allocator *allocator, string *out);
```

- Parameters: `initial`, `allocator`, `out`
- Returns: `STATUS_OK` on success, or an error status.
- Writes: created string to `*out` on success.
- Notes: when `initial` is `NULL`, creates an empty string. The string stores
  `allocator` in its header and reuses it for growth and destroy. When
  `allocator` is `NULL`, uses the C library heap through
  `alloc`/`resize`.
- Example:

```c
string string = NULL;
if (string_create("hello", NULL, &string) != STATUS_OK) {
    /* handle allocation failure */
}
```

### string_append

```c
status string_append(string *string, const char *suffix);
```

- Parameters: `string`, `suffix`
- Returns: `STATUS_OK` on success, or an error status.
- Notes: may reallocate and update `*string`.
- Example:

```c
if (string_append(&string, " world") != STATUS_OK) {
    /* handle allocation failure */
}
```

### string_prepend

```c
status string_prepend(string *string, const char *prefix);
```

- Parameters: `string`, `prefix`
- Returns: `STATUS_OK` on success, or an error status.
- Notes: may reallocate and update `*string`.
- Example:

```c
if (string_prepend(&string, "say ") != STATUS_OK) {
    /* handle allocation failure */
}
```

### string_contains

```c
bool string_contains(const string string, const char *needle);
```

- Parameters: `string`, `needle`
- Returns: `true` when `needle` appears in `string`; otherwise `false`.
- Notes: an empty `needle` matches.
- Example:

```c
bool found = string_contains(string, "hello");
```

### string_starts_with

```c
bool string_starts_with(const string string, const char *prefix);
```

- Parameters: `string`, `prefix`
- Returns: `true` when `string` begins with `prefix`; otherwise `false`.
- Notes: an empty `prefix` matches.
- Example:

```c
bool ok = string_starts_with(string, "say");
```

### string_ends_with

```c
bool string_ends_with(const string string, const char *suffix);
```

- Parameters: `string`, `suffix`
- Returns: `true` when `string` ends with `suffix`; otherwise `false`.
- Notes: an empty `suffix` matches.
- Example:

```c
bool ok = string_ends_with(string, "world");
```

### string_clear

```c
void string_clear(string string);
```

- Parameters: `string`
- Returns: none.
- Notes: resets length to zero without releasing storage.
- Example:

```c
string_clear(string);
```

### string_len

```c
size_t string_len(const string string);
```

- Parameters: `string`
- Returns: number of bytes before the terminating NUL.
- Example:

```c
size_t len = string_len(string);
```

### string_get_iterator

```c
iterator string_get_iterator(const string string);
```

- Parameters: `string`
- Returns: iterator over bytes before the terminating NUL.
- Notes: yielded pointers are `const char *`. Do not mutate or reallocate the
  string while iterating.
- Example:

```c
iterator iter = string_get_iterator(string);

const char *ch;
while ((ch = (const char *)iterator_next(&iter)) != NULL) {
    /* use *ch */
}
```

### string_destroy

```c
void string_destroy(string string);
```

- Parameters: `string`
- Returns: none.
- Notes: releases the string allocation. Do not use `string` after this call.
- Example:

```c
string_destroy(string);
```
