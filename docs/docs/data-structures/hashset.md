# ds.hashset

## DESCRIPTION

The hashset module provides fixed-size element storage with unique elements.
Hashing uses an internal FNV-1a function over stored element bytes. Element
equality uses byte comparison when `elem_eq` is `NULL`, or a caller callback
when one is provided.

Elements are copied into set-managed storage. Duplicate inserts leave the set
unchanged. Collisions are resolved with bucket chains backed by intrusive linked
list nodes.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

## FUNCTIONS

### hashset_for_each

```c
#define hashset_for_each(type, item, set)
```

- Parameters: `type`, `item`, `set`
- Notes: assigns each element pointer to a caller-declared `const type *item`.
- Example:

```c
const uint64_t *value;
hashset_for_each(uint64_t, value, set) {
    /* use *value */
}
```

### hashset_create

```c
status hashset_create(size_t elem_size,
                            hashset_elem_eq_fn elem_eq,
                            allocator *allocator,
                            hashset **out);
```

- Parameters: `elem_size`, `elem_eq`, `allocator`, `out`
- Returns: `STATUS_OK` on success, or an error status.
- Writes: opaque hashset handle to `*out` on success.
- Notes: the hashset stores `allocator` and reuses it for entries, buckets,
  rehashing, and destroy. When `allocator` is `NULL`, hashset uses the C
  library heap through `alloc`. Custom `elem_eq` callbacks must be
  consistent with the byte hash used for bucket selection.
- Example:

```c
hashset *set = NULL;
if (hashset_create(sizeof(uint64_t), NULL, NULL, &set) != STATUS_OK) {
    /* handle allocation failure */
}
```

### hashset_reserve

```c
status hashset_reserve(hashset *set, size_t size);
```

- Parameters: `set`, `size`
- Returns: `STATUS_OK` on success, or an error status.
- Notes: grows bucket storage so at least `size` elements fit without another
  rehash at the default load factor.
- Example:

```c
if (hashset_reserve(set, 1024) != STATUS_OK) {
    /* handle allocation failure */
}
```

### hashset_insert

```c
status hashset_insert(hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: `STATUS_OK` on success, or an error status.
- Notes: copies `elem` into set-managed storage when it is not already present.
- Example:

```c
uint64_t value = 42;
if (hashset_insert(set, &value) != STATUS_OK) {
    /* handle allocation failure */
}
```

### hashset_contains

```c
bool hashset_contains(const hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: `true` when an equal element exists, otherwise `false`.
- Example:

```c
bool ok = hashset_contains(set, &value);
```

### hashset_get

```c
void *hashset_get(hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: pointer to stored element in set-managed storage, or `NULL` when element is missing.
- Example:

```c
uint64_t *stored = (uint64_t *)hashset_get(set, &value);
if (stored != NULL) {
    *stored = 7;
}
```

### hashset_get_const

```c
const void *hashset_get_const(const hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: const pointer to stored element in set-managed storage, or `NULL` when element is missing.
- Example:

```c
const uint64_t *stored = (const uint64_t *)hashset_get_const(set, &value);
```

### hashset_remove

```c
void hashset_remove(hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: none.
- Notes: removes an equal element when present. Missing elements are ignored.
- Example:

```c
hashset_remove(set, &value);
```

### hashset_size

```c
size_t hashset_size(const hashset *set);
```

- Parameters: `set`
- Returns: current element count.
- Example:

```c
size_t count = hashset_size(set);
```

### hashset_get_iterator

```c
iterator hashset_get_iterator(const hashset *set);
```

- Parameters: `set`
- Returns: iterator over stored elements in bucket order.
- Notes: yielded pointers refer to set-managed storage. Do not mutate the set
  while iterating.
- Example:

```c
iterator iter = hashset_get_iterator(set);

const uint64_t *value;
while ((value = (const uint64_t *)iterator_next(&iter)) != NULL) {
    /* use *value */
}
```

### hashset_destroy

```c
void hashset_destroy(hashset *set);
```

- Parameters: `set`
- Returns: none.
- Notes: releases entries, buckets, and the opaque handle. Do not use `set` after this call.
- Example:

```c
hashset_destroy(set);
```
