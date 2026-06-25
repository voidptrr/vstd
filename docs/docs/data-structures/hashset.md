# datastruct.hashset

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

### VS_HASHSET_FOR_EACH

```c
#define VS_HASHSET_FOR_EACH(type, item, set)
```

- Parameters: `type`, `item`, `set`
- Notes: assigns each element pointer to a caller-declared `const type *item`.
- Example:

```c
const uint64_t *value;
VS_HASHSET_FOR_EACH(uint64_t, value, set) {
    /* use *value */
}
```

### vs_hashset_create

```c
vs_hashset *vs_hashset_create(size_t elem_size,
                                vs_hashset_elem_eq_fn elem_eq,
                                vs_allocator *allocator);
```

- Parameters: `elem_size`, `elem_eq`, `allocator`
- Returns: opaque hashset handle.
- Notes: the hashset stores `allocator` and reuses it for entries, buckets,
  rehashing, and destroy. When `allocator` is `NULL`, hashset uses the C
  library heap through `vs_malloc`. Custom `elem_eq` callbacks must be
  consistent with the byte hash used for bucket selection.
- Example:

```c
vs_hashset *set = vs_hashset_create(sizeof(uint64_t), NULL, NULL);
```

### vs_hashset_reserve

```c
void vs_hashset_reserve(vs_hashset *set, size_t size);
```

- Parameters: `set`, `size`
- Returns: none.
- Notes: grows bucket storage so at least `size` elements fit without another
  rehash at the default load factor.
- Example:

```c
vs_hashset_reserve(set, 1024);
```

### vs_hashset_insert

```c
void vs_hashset_insert(vs_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: none.
- Notes: copies `elem` into set-managed storage when it is not already present.
- Example:

```c
uint64_t value = 42;
vs_hashset_insert(set, &value);
```

### vs_hashset_contains

```c
bool vs_hashset_contains(const vs_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: `true` when an equal element exists, otherwise `false`.
- Example:

```c
bool ok = vs_hashset_contains(set, &value);
```

### vs_hashset_get

```c
void *vs_hashset_get(vs_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: pointer to stored element in set-managed storage, or `NULL` when element is missing.
- Example:

```c
uint64_t *stored = (uint64_t *)vs_hashset_get(set, &value);
if (stored != NULL) {
    *stored = 7;
}
```

### vs_hashset_get_const

```c
const void *vs_hashset_get_const(const vs_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: const pointer to stored element in set-managed storage, or `NULL` when element is missing.
- Example:

```c
const uint64_t *stored = (const uint64_t *)vs_hashset_get_const(set, &value);
```

### vs_hashset_remove

```c
void vs_hashset_remove(vs_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: none.
- Notes: removes an equal element when present. Missing elements are ignored.
- Example:

```c
vs_hashset_remove(set, &value);
```

### vs_hashset_size

```c
size_t vs_hashset_size(const vs_hashset *set);
```

- Parameters: `set`
- Returns: current element count.
- Example:

```c
size_t count = vs_hashset_size(set);
```

### vs_hashset_get_iterator

```c
vs_iterator vs_hashset_get_iterator(const vs_hashset *set);
```

- Parameters: `set`
- Returns: iterator over stored elements in bucket order.
- Notes: yielded pointers refer to set-managed storage. Do not mutate the set
  while iterating.
- Example:

```c
vs_iterator iter = vs_hashset_get_iterator(set);

const uint64_t *value;
while ((value = (const uint64_t *)vs_iterator_next(&iter)) != NULL) {
    /* use *value */
}
```

### vs_hashset_destroy

```c
void vs_hashset_destroy(vs_hashset *set);
```

- Parameters: `set`
- Returns: none.
- Notes: releases entries, buckets, and the opaque handle. Do not use `set` after this call.
- Example:

```c
vs_hashset_destroy(set);
```
