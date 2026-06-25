# datastruct.hashmap

## DESCRIPTION

The hashmap module provides fixed-size key/value mapping with separate chaining for collisions.
Hashing uses an internal FNV-1a function over stored key bytes. Key equality
uses byte comparison when `key_eq` is `NULL`, or a caller callback when one is
provided.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### vs_hashmap_entry_view

```c
typedef struct vs_hashmap_entry_view {
    const void *key;
    const void *value;
} vs_hashmap_entry_view;
```

Hashmap entry iterators yield pointers to this view. The view is stored inside
the iterator and is overwritten by the next `vs_iterator_next` call on that
iterator.

### vs_hashmap_iterator_type

```c
typedef enum vs_hashmap_iterator_type {
    VS_HASHMAP_ITERATOR_ENTRY,
    VS_HASHMAP_ITERATOR_KEY,
    VS_HASHMAP_ITERATOR_VALUE,
} vs_hashmap_iterator_type;
```

Selects whether `vs_hashmap_get_iterator` yields entry views, keys, or values.

## FUNCTIONS

### VS_HASHMAP_FOR_EACH_ENTRY

```c
#define VS_HASHMAP_FOR_EACH_ENTRY(item, map)
```

- Parameters: `item`, `map`
- Notes: assigns each `const vs_hashmap_entry_view *` to a caller-declared
  `item` pointer.
- Example:

```c
const vs_hashmap_entry_view *entry;
VS_HASHMAP_FOR_EACH_ENTRY(entry, map) {
    /* use entry->key and entry->value */
}
```

### VS_HASHMAP_FOR_EACH_KEY

```c
#define VS_HASHMAP_FOR_EACH_KEY(type, item, map)
```

- Parameters: `type`, `item`, `map`
- Notes: assigns each key pointer to a caller-declared `const type *item`.

### VS_HASHMAP_FOR_EACH_VALUE

```c
#define VS_HASHMAP_FOR_EACH_VALUE(type, item, map)
```

- Parameters: `type`, `item`, `map`
- Notes: assigns each value pointer to a caller-declared `const type *item`.

### vs_hashmap_create

```c
vs_hashmap *vs_hashmap_create(size_t key_size,
                                size_t value_size,
                                vs_hashmap_key_eq_fn key_eq,
                                vs_allocator *allocator);
```

- Parameters: `key_size`, `value_size`, `key_eq`, `allocator`
- Returns: opaque hashmap handle.
- Notes: the hashmap stores `allocator` and reuses it for entries, buckets,
  rehashing, and destroy. When `allocator` is `NULL`, hashmap uses the C
  library heap through `vs_malloc`. Custom `key_eq` callbacks must be
  consistent with the byte hash used for bucket selection.
- Example:

```c
vs_hashmap *map = vs_hashmap_create(sizeof(uint64_t), sizeof(uint64_t), NULL, NULL);
```

### vs_hashmap_reserve

```c
void vs_hashmap_reserve(vs_hashmap *map, size_t size);
```

- Parameters: `map`, `size`
- Returns: none.
- Notes: grows bucket storage so at least `size` entries fit without another
  rehash at the default load factor.
- Example:

```c
vs_hashmap_reserve(map, 1024);
```

### vs_hashmap_put

```c
void vs_hashmap_put(vs_hashmap *map, const void *key, const void *value);
```

- Parameters: `map`, `key`, `value`
- Returns: none.
- Example:

```c
uint64_t key = 42;
uint64_t value = 9001;
vs_hashmap_put(map, &key, &value);
```

### vs_hashmap_get

```c
void *vs_hashmap_get(vs_hashmap *map, const void *key);
```

- Parameters: `map`, `key`
- Returns: pointer to stored value in map-managed storage, or `NULL` when key is missing.
- Example:

```c
uint64_t *value = (uint64_t *)vs_hashmap_get(map, &key);
if (value != NULL) {
    *value = 7;
}
```

### vs_hashmap_get_const

```c
const void *vs_hashmap_get_const(const vs_hashmap *map, const void *key);
```

- Parameters: `map`, `key`
- Returns: pointer to stored value in map-managed storage, or `NULL` when key is missing.
- Example:

```c
const uint64_t *value = (const uint64_t *)vs_hashmap_get_const(map, &key);
```

### vs_hashmap_remove

```c
void vs_hashmap_remove(vs_hashmap *map, const void *key);
```

- Parameters: `map`, `key`
- Returns: none.
- Notes: missing keys are ignored.
- Example:

```c
vs_hashmap_remove(map, &key);
```

### vs_hashmap_size

```c
size_t vs_hashmap_size(const vs_hashmap *map);
```

- Parameters: `map`
- Returns: current entry count.
- Example:

```c
size_t count = vs_hashmap_size(map);
```

### vs_hashmap_get_iterator

```c
vs_iterator vs_hashmap_get_iterator(const vs_hashmap *map,
                                    vs_hashmap_iterator_type type);
```

- Parameters: `map`, `type`
- Returns: iterator over entry views, keys, or values in bucket order.
- Notes: do not mutate the hashmap while iterating. Entry views are stored
  inside the iterator and are overwritten by the next `vs_iterator_next` call.
- Example:

```c
vs_iterator iter = vs_hashmap_get_iterator(map, VS_HASHMAP_ITERATOR_ENTRY);

const vs_hashmap_entry_view *entry;
while ((entry = (const vs_hashmap_entry_view *)vs_iterator_next(&iter)) != NULL) {
    const uint64_t *key = (const uint64_t *)entry->key;
    const uint64_t *value = (const uint64_t *)entry->value;
}
```

Key iterator example:

```c
vs_iterator iter = vs_hashmap_get_iterator(map, VS_HASHMAP_ITERATOR_KEY);

const uint64_t *key;
while ((key = (const uint64_t *)vs_iterator_next(&iter)) != NULL) {
    /* use *key */
}
```

Value iterator example:

```c
vs_iterator iter = vs_hashmap_get_iterator(map, VS_HASHMAP_ITERATOR_VALUE);

const uint64_t *value;
while ((value = (const uint64_t *)vs_iterator_next(&iter)) != NULL) {
    /* use *value */
}
```

### vs_hashmap_destroy

```c
void vs_hashmap_destroy(vs_hashmap *map);
```

- Parameters: `map`
- Returns: none.
- Notes: releases entries, buckets, and the opaque handle. Do not use `map` after this call.
- Example:

```c
vs_hashmap_destroy(map);
```
