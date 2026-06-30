# ds.k4c_string

## DESCRIPTION

The k4c_string module provides a growable NUL-terminated k4c_string. The public type is
`char *`, so it can be passed to C APIs that expect a normal k4c_string. Metadata is
stored before the visible character buffer.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### k4c_string

```c
typedef char *k4c_string;
```

## FUNCTIONS

### k4c_string_create

```c
k4c_status k4c_string_create(const char *initial, k4c_allocator *k4c_allocator, k4c_string *out);
```

- Parameters: `initial`, `k4c_allocator`, `out`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Writes: created k4c_string to `*out` on success.
- Notes: when `initial` is `NULL`, creates an empty k4c_string. The k4c_string stores
  `k4c_allocator` in its header and reuses it for growth and destroy. When
  `k4c_allocator` is `NULL`, uses the C library k4c_heap through
  `k4c_alloc`/`k4c_resize`.
- Example:

```c
k4c_string k4c_string = NULL;
if (k4c_string_create("hello", NULL, &k4c_string) != K4C_STATUS_OK) {
    /* handle allocation failure */
}
```

### k4c_string_append

```c
k4c_status k4c_string_append(k4c_string *k4c_string, const char *suffix);
```

- Parameters: `k4c_string`, `suffix`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Notes: may reallocate and update `*k4c_string`.
- Example:

```c
if (k4c_string_append(&k4c_string, " world") != K4C_STATUS_OK) {
    /* handle allocation failure */
}
```

### k4c_string_prepend

```c
k4c_status k4c_string_prepend(k4c_string *k4c_string, const char *prefix);
```

- Parameters: `k4c_string`, `prefix`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Notes: may reallocate and update `*k4c_string`.
- Example:

```c
if (k4c_string_prepend(&k4c_string, "say ") != K4C_STATUS_OK) {
    /* handle allocation failure */
}
```

### k4c_string_contains

```c
bool k4c_string_contains(const k4c_string k4c_string, const char *needle);
```

- Parameters: `k4c_string`, `needle`
- Returns: `true` when `needle` appears in `k4c_string`; otherwise `false`.
- Notes: an empty `needle` matches.
- Example:

```c
bool found = k4c_string_contains(k4c_string, "hello");
```

### k4c_string_starts_with

```c
bool k4c_string_starts_with(const k4c_string k4c_string, const char *prefix);
```

- Parameters: `k4c_string`, `prefix`
- Returns: `true` when `k4c_string` begins with `prefix`; otherwise `false`.
- Notes: an empty `prefix` matches.
- Example:

```c
bool ok = k4c_string_starts_with(k4c_string, "say");
```

### k4c_string_ends_with

```c
bool k4c_string_ends_with(const k4c_string k4c_string, const char *suffix);
```

- Parameters: `k4c_string`, `suffix`
- Returns: `true` when `k4c_string` ends with `suffix`; otherwise `false`.
- Notes: an empty `suffix` matches.
- Example:

```c
bool ok = k4c_string_ends_with(k4c_string, "world");
```

### k4c_string_clear

```c
void k4c_string_clear(k4c_string k4c_string);
```

- Parameters: `k4c_string`
- Returns: none.
- Notes: resets length to zero without releasing storage.
- Example:

```c
k4c_string_clear(k4c_string);
```

### k4c_string_len

```c
size_t k4c_string_len(const k4c_string k4c_string);
```

- Parameters: `k4c_string`
- Returns: number of bytes before the terminating NUL.
- Example:

```c
size_t len = k4c_string_len(k4c_string);
```

### k4c_string_get_iterator

```c
k4c_iterator k4c_string_get_iterator(const k4c_string k4c_string);
```

- Parameters: `k4c_string`
- Returns: k4c_iterator over bytes before the terminating NUL.
- Notes: yielded pointers are `const char *`. Do not mutate or reallocate the
  k4c_string while iterating.
- Example:

```c
k4c_iterator iter = k4c_string_get_iterator(k4c_string);

const char *ch;
while ((ch = (const char *)k4c_iterator_next(&iter)) != NULL) {
    /* use *ch */
}
```

### k4c_string_destroy

```c
void k4c_string_destroy(k4c_string k4c_string);
```

- Parameters: `k4c_string`
- Returns: none.
- Notes: releases the k4c_string allocation. Do not use `k4c_string` after this call.
- Example:

```c
k4c_string_destroy(k4c_string);
```
