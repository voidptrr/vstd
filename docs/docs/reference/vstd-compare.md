# compare

Shared equality and ordering callbacks for containers.

## Callback types

### vs_eq_fn

```c
typedef bool (*vs_eq_fn)(const void *lhs, const void *rhs, size_t size);
```

- Returns: `true` when values are equal.

### vs_cmp_fn

```c
typedef int (*vs_cmp_fn)(const void *lhs, const void *rhs);
```

- Returns: negative when `lhs < rhs`, zero when equal, positive when `lhs > rhs`.

## Equality helpers

### vs_eq_bytes

```c
bool vs_eq_bytes(const void *lhs, const void *rhs, size_t size);
```

- Parameters: `lhs`, `rhs`, `size`
- Returns: `true` when byte regions are equal.

### vs_eq_cstr

```c
bool vs_eq_cstr(const void *lhs, const void *rhs, size_t size);
```

- Parameters: `lhs`, `rhs`, `size`
- Returns: `true` when null-terminated strings are equal.
- Notes: `lhs` and `rhs` are inline character buffers. `size` is ignored.

### vs_eq_cstr_ptr

```c
bool vs_eq_cstr_ptr(const void *lhs, const void *rhs, size_t size);
```

- Parameters: `lhs`, `rhs`, `size`
- Returns: `true` when pointed-to null-terminated strings are equal.
- Notes: `lhs` and `rhs` are pointers to stored `const char *` values. `size` is ignored.
- Warning: this helper only defines equality. Hash maps also need a matching hash over string
  contents; `vs_hashmap` currently hashes stored key bytes.

### vs_eq_i32 / vs_eq_i64 / vs_eq_u32 / vs_eq_u64 / vs_eq_size

```c
bool vs_eq_i32(const void *lhs, const void *rhs, size_t size);
bool vs_eq_i64(const void *lhs, const void *rhs, size_t size);
bool vs_eq_u32(const void *lhs, const void *rhs, size_t size);
bool vs_eq_u64(const void *lhs, const void *rhs, size_t size);
bool vs_eq_size(const void *lhs, const void *rhs, size_t size);
```

- Parameters: `lhs`, `rhs`, `size`
- Returns: `true` when scalar values are equal.
- Notes: `size` is ignored.

## Ordering helpers

### vs_cmp_i32 / vs_cmp_i64 / vs_cmp_u32 / vs_cmp_u64 / vs_cmp_size

```c
int vs_cmp_i32(const void *lhs, const void *rhs);
int vs_cmp_i64(const void *lhs, const void *rhs);
int vs_cmp_u32(const void *lhs, const void *rhs);
int vs_cmp_u64(const void *lhs, const void *rhs);
int vs_cmp_size(const void *lhs, const void *rhs);
```

- Parameters: `lhs`, `rhs`
- Returns: negative when `lhs < rhs`, zero when equal, positive when `lhs > rhs`.
