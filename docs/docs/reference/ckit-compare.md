# compare

Shared equality and ordering callbacks for containers.

## Callback types

### ck_eq_fn

```c
typedef bool (*ck_eq_fn)(const void *lhs, const void *rhs, size_t size);
```

- Returns: `true` when values are equal.

### ck_cmp_fn

```c
typedef int (*ck_cmp_fn)(const void *lhs, const void *rhs);
```

- Returns: negative when `lhs < rhs`, zero when equal, positive when `lhs > rhs`.

## Equality helpers

### ck_eq_bytes

```c
bool ck_eq_bytes(const void *lhs, const void *rhs, size_t size);
```

- Parameters: `lhs`, `rhs`, `size`
- Returns: `true` when byte regions are equal.

### ck_eq_cstr

```c
bool ck_eq_cstr(const void *lhs, const void *rhs, size_t size);
```

- Parameters: `lhs`, `rhs`, `size`
- Returns: `true` when null-terminated strings are equal.
- Notes: `lhs` and `rhs` are inline character buffers. `size` is ignored.

### ck_eq_cstr_ptr

```c
bool ck_eq_cstr_ptr(const void *lhs, const void *rhs, size_t size);
```

- Parameters: `lhs`, `rhs`, `size`
- Returns: `true` when pointed-to null-terminated strings are equal.
- Notes: `lhs` and `rhs` are pointers to stored `const char *` values. `size` is ignored.
- Warning: this helper only defines equality. Hash maps also need a matching hash over string
  contents; `ck_hashmap` currently hashes stored key bytes.

### ck_eq_i32 / ck_eq_i64 / ck_eq_u32 / ck_eq_u64 / ck_eq_size

```c
bool ck_eq_i32(const void *lhs, const void *rhs, size_t size);
bool ck_eq_i64(const void *lhs, const void *rhs, size_t size);
bool ck_eq_u32(const void *lhs, const void *rhs, size_t size);
bool ck_eq_u64(const void *lhs, const void *rhs, size_t size);
bool ck_eq_size(const void *lhs, const void *rhs, size_t size);
```

- Parameters: `lhs`, `rhs`, `size`
- Returns: `true` when scalar values are equal.
- Notes: `size` is ignored.

## Ordering helpers

### ck_cmp_i32 / ck_cmp_i64 / ck_cmp_u32 / ck_cmp_u64 / ck_cmp_size

```c
int ck_cmp_i32(const void *lhs, const void *rhs);
int ck_cmp_i64(const void *lhs, const void *rhs);
int ck_cmp_u32(const void *lhs, const void *rhs);
int ck_cmp_u64(const void *lhs, const void *rhs);
int ck_cmp_size(const void *lhs, const void *rhs);
```

- Parameters: `lhs`, `rhs`
- Returns: negative when `lhs < rhs`, zero when equal, positive when `lhs > rhs`.
