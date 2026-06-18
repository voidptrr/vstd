# datastruct.hashmap

## DESCRIPTION

The hashmap module provides fixed-size key/value mapping with separate chaining for collisions.
Hashing uses an internal FNV-1a function over stored key bytes. Key equality
uses byte comparison when `key_eq` is `NULL`, or a caller callback when one is
provided.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## FUNCTIONS

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

### vs_hashmap_put

```c
void vs_hashmap_put(vs_hashmap *map, const void *key, const void *value);
```

- Parameters: `map`, `key`, `value`
- Returns: none.

### vs_hashmap_get

```c
void *vs_hashmap_get(vs_hashmap *map, const void *key);
```

- Parameters: `map`, `key`
- Returns: pointer to stored value in map-managed storage, or `NULL` when key is missing.

### vs_hashmap_get_const

```c
const void *vs_hashmap_get_const(const vs_hashmap *map, const void *key);
```

- Parameters: `map`, `key`
- Returns: pointer to stored value in map-managed storage, or `NULL` when key is missing.

### vs_hashmap_remove

```c
void vs_hashmap_remove(vs_hashmap *map, const void *key);
```

- Parameters: `map`, `key`
- Returns: none.
- Notes: missing keys are ignored.

### vs_hashmap_size

```c
size_t vs_hashmap_size(const vs_hashmap *map);
```

- Parameters: `map`
- Returns: current entry count.

### vs_hashmap_destroy

```c
void vs_hashmap_destroy(vs_hashmap *map);
```

- Parameters: `map`
- Returns: none.
- Notes: releases entries, buckets, and the opaque handle. Do not use `map` after this call.

## EXAMPLE

```c
#include <vstd/datastruct/hashmap.h>
#include <stdint.h>

int main(void) {
    int status = 0;
    vs_hashmap *map;
    uint64_t key = 42;
    uint64_t value = 9001;
    uint64_t *found = NULL;

    map = vs_hashmap_create(sizeof(uint64_t), sizeof(uint64_t), NULL, NULL);
    vs_hashmap_put(map, &key, &value);

    found = (uint64_t *)vs_hashmap_get(map, &key);
    if (found == NULL || *found != value) {
        status = 1;
        goto cleanup;
    }

    vs_hashmap_remove(map, &key);
    if (vs_hashmap_get(map, &key) != NULL) {
        status = 1;
        goto cleanup;
    }

cleanup:
    vs_hashmap_destroy(map);
    return status;
}
```
