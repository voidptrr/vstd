# ds.k4c_hashmap

## DESCRIPTION

The k4c_hashmap module provides fixed-size key/value mapping with separate chaining for collisions.
Hashing uses an internal FNV-1a function over stored key bytes. Key equality
uses byte comparison when `key_eq` is `NULL`, or a caller callback when one is
provided.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### k4c_hashmap_entry_view

```c
typedef struct k4c_hashmap_entry_view {
    const void *key;
    const void *value;
} k4c_hashmap_entry_view;
```

Hashmap entry iterators yield pointers to this view. The view is stored inside
the k4c_iterator and is overwritten by the next `k4c_iterator_next` call on that
k4c_iterator.

### k4c_hashmap_iterator_type

```c
typedef enum k4c_hashmap_iterator_type {
    K4C_HASHMAP_ITERATOR_ENTRY,
    K4C_HASHMAP_ITERATOR_KEY,
    K4C_HASHMAP_ITERATOR_VALUE,
} k4c_hashmap_iterator_type;
```

Selects whether `k4c_hashmap_get_iterator` yields entry views, keys, or values.

## FUNCTIONS

### k4c_hashmap_for_each_entry

```c
#define k4c_hashmap_for_each_entry(item, map)
```

- Parameters: `item`, `map`
- Notes: assigns each `const k4c_hashmap_entry_view *` to a caller-declared
  `item` pointer.
- Example:

```c
const k4c_hashmap_entry_view *entry;
k4c_hashmap_for_each_entry(entry, map) {
    /* use entry->key and entry->value */
}
```

### k4c_hashmap_for_each_key

```c
#define k4c_hashmap_for_each_key(type, item, map)
```

- Parameters: `type`, `item`, `map`
- Notes: assigns each key pointer to a caller-declared `const type *item`.

### k4c_hashmap_for_each_value

```c
#define k4c_hashmap_for_each_value(type, item, map)
```

- Parameters: `type`, `item`, `map`
- Notes: assigns each value pointer to a caller-declared `const type *item`.

### k4c_hashmap_create

```c
k4c_status k4c_hashmap_create(size_t key_size,
                            size_t value_size,
                            k4c_hashmap_key_eq_fn key_eq,
                            k4c_allocator *k4c_allocator,
                            k4c_hashmap **out);
```

- Parameters: `key_size`, `value_size`, `key_eq`, `k4c_allocator`, `out`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Writes: opaque k4c_hashmap handle to `*out` on success.
- Notes: the k4c_hashmap copies `k4c_allocator` and reuses it for entries, buckets,
  rehashing, and destroy. When `k4c_allocator` is `NULL`, k4c_hashmap uses the C
  library k4c_heap through `k4c_alloc`. Custom `key_eq` callbacks must be
  consistent with the byte hash used for bucket selection.
- Example:

```c
k4c_hashmap *map = NULL;
if (k4c_hashmap_create(sizeof(uint64_t), sizeof(uint64_t), NULL, NULL, &map) != K4C_STATUS_OK) {
    /* handle allocation failure */
}
```

### k4c_hashmap_reserve

```c
k4c_status k4c_hashmap_reserve(k4c_hashmap *map, size_t size);
```

- Parameters: `map`, `size`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Notes: grows bucket storage so at least `size` entries fit without another
  rehash at the default load factor.
- Example:

```c
if (k4c_hashmap_reserve(map, 1024) != K4C_STATUS_OK) {
    /* handle allocation failure */
}
```

### k4c_hashmap_put

```c
k4c_status k4c_hashmap_put(k4c_hashmap *map, const void *key, const void *value);
```

- Parameters: `map`, `key`, `value`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Example:

```c
uint64_t key = 42;
uint64_t value = 9001;
if (k4c_hashmap_put(map, &key, &value) != K4C_STATUS_OK) {
    /* handle allocation failure */
}
```

### k4c_hashmap_get

```c
void *k4c_hashmap_get(const k4c_hashmap *map, const void *key);
```

- Parameters: `map`, `key`
- Returns: pointer to stored value in map-managed storage, or `NULL` when key is missing.
- Example:

```c
uint64_t *value = (uint64_t *)k4c_hashmap_get(map, &key);
if (value != NULL) {
    *value = 7;
}
```

### k4c_hashmap_remove

```c
void k4c_hashmap_remove(k4c_hashmap *map, const void *key);
```

- Parameters: `map`, `key`
- Returns: none.
- Notes: missing keys are ignored.
- Example:

```c
k4c_hashmap_remove(map, &key);
```

### k4c_hashmap_size

```c
size_t k4c_hashmap_size(const k4c_hashmap *map);
```

- Parameters: `map`
- Returns: current entry count.
- Example:

```c
size_t count = k4c_hashmap_size(map);
```

### k4c_hashmap_get_iterator

```c
k4c_iterator k4c_hashmap_get_iterator(const k4c_hashmap *map,
                                    k4c_hashmap_iterator_type type);
```

- Parameters: `map`, `type`
- Returns: k4c_iterator over entry views, keys, or values in bucket order.
- Notes: do not mutate the k4c_hashmap while iterating. Entry views are stored
  inside the k4c_iterator and are overwritten by the next `k4c_iterator_next` call.
- Example:

```c
k4c_iterator iter = k4c_hashmap_get_iterator(map, K4C_HASHMAP_ITERATOR_ENTRY);

const k4c_hashmap_entry_view *entry;
while ((entry = (const k4c_hashmap_entry_view *)k4c_iterator_next(&iter)) != NULL) {
    const uint64_t *key = (const uint64_t *)entry->key;
    const uint64_t *value = (const uint64_t *)entry->value;
}
```

Key k4c_iterator example:

```c
k4c_iterator iter = k4c_hashmap_get_iterator(map, K4C_HASHMAP_ITERATOR_KEY);

const uint64_t *key;
while ((key = (const uint64_t *)k4c_iterator_next(&iter)) != NULL) {
    /* use *key */
}
```

Value k4c_iterator example:

```c
k4c_iterator iter = k4c_hashmap_get_iterator(map, K4C_HASHMAP_ITERATOR_VALUE);

const uint64_t *value;
while ((value = (const uint64_t *)k4c_iterator_next(&iter)) != NULL) {
    /* use *value */
}
```

### k4c_hashmap_destroy

```c
void k4c_hashmap_destroy(k4c_hashmap *map);
```

- Parameters: `map`
- Returns: none.
- Notes: releases entries, buckets, and the opaque handle. Do not use `map` after this call.
- Example:

```c
k4c_hashmap_destroy(map);
```
