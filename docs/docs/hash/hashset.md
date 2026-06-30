# ds.k4c_hashset

## DESCRIPTION

The k4c_hashset module provides fixed-size element storage with unique elements.
Hashing uses an internal FNV-1a function over stored element bytes. Element
equality uses byte comparison when `elem_eq` is `NULL`, or a caller callback
when one is provided.

Elements are copied into set-managed storage. Duplicate inserts leave the set
unchanged. Collisions are resolved with bucket chains backed by intrusive linked
list nodes.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

## FUNCTIONS

### k4c_hashset_for_each

```c
#define k4c_hashset_for_each(type, item, set)
```

- Parameters: `type`, `item`, `set`
- Notes: assigns each element pointer to a caller-declared `const type *item`.
- Example:

```c
const uint64_t *value;
k4c_hashset_for_each(uint64_t, value, set) {
    /* use *value */
}
```

### k4c_hashset_create

```c
k4c_status k4c_hashset_create(size_t elem_size,
                            k4c_hashset_elem_eq_fn elem_eq,
                            k4c_allocator *k4c_allocator,
                            k4c_hashset **out);
```

- Parameters: `elem_size`, `elem_eq`, `k4c_allocator`, `out`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Writes: opaque k4c_hashset handle to `*out` on success.
- Notes: the k4c_hashset copies `k4c_allocator` and reuses it for entries, buckets,
  rehashing, and destroy. When `k4c_allocator` is `NULL`, k4c_hashset uses the C
  library k4c_heap through `k4c_alloc`. Custom `elem_eq` callbacks must be
  consistent with the byte hash used for bucket selection.
- Example:

```c
k4c_hashset *set = NULL;
if (k4c_hashset_create(sizeof(uint64_t), NULL, NULL, &set) != K4C_STATUS_OK) {
    /* handle allocation failure */
}
```

### k4c_hashset_reserve

```c
k4c_status k4c_hashset_reserve(k4c_hashset *set, size_t size);
```

- Parameters: `set`, `size`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Notes: grows bucket storage so at least `size` elements fit without another
  rehash at the default load factor.
- Example:

```c
if (k4c_hashset_reserve(set, 1024) != K4C_STATUS_OK) {
    /* handle allocation failure */
}
```

### k4c_hashset_insert

```c
k4c_status k4c_hashset_insert(k4c_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Notes: copies `elem` into set-managed storage when it is not already present.
- Example:

```c
uint64_t value = 42;
if (k4c_hashset_insert(set, &value) != K4C_STATUS_OK) {
    /* handle allocation failure */
}
```

### k4c_hashset_contains

```c
bool k4c_hashset_contains(const k4c_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: `true` when an equal element exists, otherwise `false`.
- Example:

```c
bool ok = k4c_hashset_contains(set, &value);
```

### k4c_hashset_get

```c
void *k4c_hashset_get(const k4c_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: pointer to stored element in set-managed storage, or `NULL` when element is missing.
- Example:

```c
uint64_t *stored = (uint64_t *)k4c_hashset_get(set, &value);
if (stored != NULL) {
    *stored = 7;
}
```

### k4c_hashset_remove

```c
void k4c_hashset_remove(k4c_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: none.
- Notes: removes an equal element when present. Missing elements are ignored.
- Example:

```c
k4c_hashset_remove(set, &value);
```

### k4c_hashset_size

```c
size_t k4c_hashset_size(const k4c_hashset *set);
```

- Parameters: `set`
- Returns: current element count.
- Example:

```c
size_t count = k4c_hashset_size(set);
```

### k4c_hashset_get_iterator

```c
k4c_iterator k4c_hashset_get_iterator(const k4c_hashset *set);
```

- Parameters: `set`
- Returns: k4c_iterator over stored elements in bucket order.
- Notes: yielded pointers refer to set-managed storage. Do not mutate the set
  while iterating.
- Example:

```c
k4c_iterator iter = k4c_hashset_get_iterator(set);

const uint64_t *value;
while ((value = (const uint64_t *)k4c_iterator_next(&iter)) != NULL) {
    /* use *value */
}
```

### k4c_hashset_destroy

```c
void k4c_hashset_destroy(k4c_hashset *set);
```

- Parameters: `set`
- Returns: none.
- Notes: releases entries, buckets, and the opaque handle. Do not use `set` after this call.
- Example:

```c
k4c_hashset_destroy(set);
```
