# ds.hashmap

## DESCRIPTION

The hashmap module provides fixed-size key/value mapping with separate chaining for collisions.
Hashing uses an internal FNV-1a function over stored key bytes. Key equality
uses byte comparison when `key_eq` is `NULL`, or a caller callback when one is
provided.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### hashmap_entry_view

```c
typedef struct hashmap_entry_view {
    const void *key;
    const void *value;
} hashmap_entry_view;
```

Hashmap entry iterators yield pointers to this view. The view is stored inside
the iterator and is overwritten by the next `iterator_next` call on that
iterator.

### hashmap_iterator_type

```c
typedef enum hashmap_iterator_type {
    HASHMAP_ITERATOR_ENTRY,
    HASHMAP_ITERATOR_KEY,
    HASHMAP_ITERATOR_VALUE,
} hashmap_iterator_type;
```

Selects whether `hashmap_get_iterator` yields entry views, keys, or values.

## FUNCTIONS

### hashmap_for_each_entry

```c
#define hashmap_for_each_entry(item, map)
```

- Parameters: `item`, `map`
- Notes: assigns each `const hashmap_entry_view *` to a caller-declared
  `item` pointer.
- Example:

```c
const hashmap_entry_view *entry;
hashmap_for_each_entry(entry, map) {
    /* use entry->key and entry->value */
}
```

### hashmap_for_each_key

```c
#define hashmap_for_each_key(type, item, map)
```

- Parameters: `type`, `item`, `map`
- Notes: assigns each key pointer to a caller-declared `const type *item`.

### hashmap_for_each_value

```c
#define hashmap_for_each_value(type, item, map)
```

- Parameters: `type`, `item`, `map`
- Notes: assigns each value pointer to a caller-declared `const type *item`.

### hashmap_create

```c
status hashmap_create(size_t key_size,
                            size_t value_size,
                            hashmap_key_eq_fn key_eq,
                            allocator *allocator,
                            hashmap **out);
```

- Parameters: `key_size`, `value_size`, `key_eq`, `allocator`, `out`
- Returns: `STATUS_OK` on success, or an error status.
- Writes: opaque hashmap handle to `*out` on success.
- Notes: the hashmap stores `allocator` and reuses it for entries, buckets,
  rehashing, and destroy. When `allocator` is `NULL`, hashmap uses the C
  library heap through `alloc`. Custom `key_eq` callbacks must be
  consistent with the byte hash used for bucket selection.
- Example:

```c
hashmap *map = NULL;
if (hashmap_create(sizeof(uint64_t), sizeof(uint64_t), NULL, NULL, &map) != STATUS_OK) {
    /* handle allocation failure */
}
```

### hashmap_reserve

```c
status hashmap_reserve(hashmap *map, size_t size);
```

- Parameters: `map`, `size`
- Returns: `STATUS_OK` on success, or an error status.
- Notes: grows bucket storage so at least `size` entries fit without another
  rehash at the default load factor.
- Example:

```c
if (hashmap_reserve(map, 1024) != STATUS_OK) {
    /* handle allocation failure */
}
```

### hashmap_put

```c
status hashmap_put(hashmap *map, const void *key, const void *value);
```

- Parameters: `map`, `key`, `value`
- Returns: `STATUS_OK` on success, or an error status.
- Example:

```c
uint64_t key = 42;
uint64_t value = 9001;
if (hashmap_put(map, &key, &value) != STATUS_OK) {
    /* handle allocation failure */
}
```

### hashmap_get

```c
void *hashmap_get(hashmap *map, const void *key);
```

- Parameters: `map`, `key`
- Returns: pointer to stored value in map-managed storage, or `NULL` when key is missing.
- Example:

```c
uint64_t *value = (uint64_t *)hashmap_get(map, &key);
if (value != NULL) {
    *value = 7;
}
```

### hashmap_get_const

```c
const void *hashmap_get_const(const hashmap *map, const void *key);
```

- Parameters: `map`, `key`
- Returns: pointer to stored value in map-managed storage, or `NULL` when key is missing.
- Example:

```c
const uint64_t *value = (const uint64_t *)hashmap_get_const(map, &key);
```

### hashmap_remove

```c
void hashmap_remove(hashmap *map, const void *key);
```

- Parameters: `map`, `key`
- Returns: none.
- Notes: missing keys are ignored.
- Example:

```c
hashmap_remove(map, &key);
```

### hashmap_size

```c
size_t hashmap_size(const hashmap *map);
```

- Parameters: `map`
- Returns: current entry count.
- Example:

```c
size_t count = hashmap_size(map);
```

### hashmap_get_iterator

```c
iterator hashmap_get_iterator(const hashmap *map,
                                    hashmap_iterator_type type);
```

- Parameters: `map`, `type`
- Returns: iterator over entry views, keys, or values in bucket order.
- Notes: do not mutate the hashmap while iterating. Entry views are stored
  inside the iterator and are overwritten by the next `iterator_next` call.
- Example:

```c
iterator iter = hashmap_get_iterator(map, HASHMAP_ITERATOR_ENTRY);

const hashmap_entry_view *entry;
while ((entry = (const hashmap_entry_view *)iterator_next(&iter)) != NULL) {
    const uint64_t *key = (const uint64_t *)entry->key;
    const uint64_t *value = (const uint64_t *)entry->value;
}
```

Key iterator example:

```c
iterator iter = hashmap_get_iterator(map, HASHMAP_ITERATOR_KEY);

const uint64_t *key;
while ((key = (const uint64_t *)iterator_next(&iter)) != NULL) {
    /* use *key */
}
```

Value iterator example:

```c
iterator iter = hashmap_get_iterator(map, HASHMAP_ITERATOR_VALUE);

const uint64_t *value;
while ((value = (const uint64_t *)iterator_next(&iter)) != NULL) {
    /* use *value */
}
```

### hashmap_destroy

```c
void hashmap_destroy(hashmap *map);
```

- Parameters: `map`
- Returns: none.
- Notes: releases entries, buckets, and the opaque handle. Do not use `map` after this call.
- Example:

```c
hashmap_destroy(map);
```
