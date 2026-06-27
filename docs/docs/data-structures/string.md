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
vs_status vs_string_create(const char *initial, vs_allocator *allocator, vs_string *out);
```

- Parameters: `initial`, `allocator`, `out`
- Returns: `VS_STATUS_OK` on success, or an error status.
- Writes: created string to `*out` on success.
- Notes: when `initial` is `NULL`, creates an empty string. The string stores
  `allocator` in its header and reuses it for growth and destroy. When
  `allocator` is `NULL`, uses the C library heap through
  `vs_alloc`/`vs_resize`.
- Example:

```c
vs_string string = NULL;
if (vs_string_create("hello", NULL, &string) != VS_STATUS_OK) {
    /* handle allocation failure */
}
```

### vs_string_append

```c
vs_status vs_string_append(vs_string *string, const char *suffix);
```

- Parameters: `string`, `suffix`
- Returns: `VS_STATUS_OK` on success, or an error status.
- Notes: may reallocate and update `*string`.
- Example:

```c
if (vs_string_append(&string, " world") != VS_STATUS_OK) {
    /* handle allocation failure */
}
```

### vs_string_prepend

```c
vs_status vs_string_prepend(vs_string *string, const char *prefix);
```

- Parameters: `string`, `prefix`
- Returns: `VS_STATUS_OK` on success, or an error status.
- Notes: may reallocate and update `*string`.
- Example:

```c
if (vs_string_prepend(&string, "say ") != VS_STATUS_OK) {
    /* handle allocation failure */
}
```

### vs_string_contains

```c
bool vs_string_contains(const vs_string string, const char *needle);
```

- Parameters: `string`, `needle`
- Returns: `true` when `needle` appears in `string`; otherwise `false`.
- Notes: an empty `needle` matches.
- Example:

```c
bool found = vs_string_contains(string, "hello");
```

### vs_string_starts_with

```c
bool vs_string_starts_with(const vs_string string, const char *prefix);
```

- Parameters: `string`, `prefix`
- Returns: `true` when `string` begins with `prefix`; otherwise `false`.
- Notes: an empty `prefix` matches.
- Example:

```c
bool ok = vs_string_starts_with(string, "say");
```

### vs_string_ends_with

```c
bool vs_string_ends_with(const vs_string string, const char *suffix);
```

- Parameters: `string`, `suffix`
- Returns: `true` when `string` ends with `suffix`; otherwise `false`.
- Notes: an empty `suffix` matches.
- Example:

```c
bool ok = vs_string_ends_with(string, "world");
```

### vs_string_clear

```c
void vs_string_clear(vs_string string);
```

- Parameters: `string`
- Returns: none.
- Notes: resets length to zero without releasing storage.
- Example:

```c
vs_string_clear(string);
```

### vs_string_len

```c
size_t vs_string_len(const vs_string string);
```

- Parameters: `string`
- Returns: number of bytes before the terminating NUL.
- Example:

```c
size_t len = vs_string_len(string);
```

### vs_string_get_iterator

```c
vs_iterator vs_string_get_iterator(const vs_string string);
```

- Parameters: `string`
- Returns: iterator over bytes before the terminating NUL.
- Notes: yielded pointers are `const char *`. Do not mutate or reallocate the
  string while iterating.
- Example:

```c
vs_iterator iter = vs_string_get_iterator(string);

const char *ch;
while ((ch = (const char *)vs_iterator_next(&iter)) != NULL) {
    /* use *ch */
}
```

### vs_string_destroy

```c
void vs_string_destroy(vs_string string);
```

- Parameters: `string`
- Returns: none.
- Notes: releases the string allocation. Do not use `string` after this call.
- Example:

```c
vs_string_destroy(string);
```
