# datastruct.hashmap

## DESCRIPTION

The hashmap module provides fixed-size key/value mapping with separate chaining for collisions.
Hashing uses an internal FNV-1a function; key equality is provided by a caller callback.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## FUNCTIONS

### ck_hashmap_create

```c
ck_hashmap *ck_hashmap_create(size_t key_size,
                                size_t value_size,
                                ck_hashmap_key_eq_fn key_eq,
                                ck_allocator *allocator);
```

- Parameters: `key_size`, `value_size`, `key_eq`, `allocator`
- Returns: opaque hashmap handle.
- Notes: the hashmap stores `allocator` and reuses it for entries, buckets,
  rehashing, and destroy. When `allocator` is `NULL`, hashmap uses the C
  library heap through `ck_malloc`.

### ck_hashmap_put

```c
void ck_hashmap_put(ck_hashmap *map, const void *key, const void *value);
```

- Parameters: `map`, `key`, `value`
- Returns: none.

### ck_hashmap_get

```c
void *ck_hashmap_get(ck_hashmap *map, const void *key);
```

- Parameters: `map`, `key`
- Returns: pointer to stored value in map-managed storage, or `NULL` when key is missing.

### ck_hashmap_get_const

```c
const void *ck_hashmap_get_const(const ck_hashmap *map, const void *key);
```

- Parameters: `map`, `key`
- Returns: pointer to stored value in map-managed storage, or `NULL` when key is missing.

### ck_hashmap_remove

```c
void ck_hashmap_remove(ck_hashmap *map, const void *key);
```

- Parameters: `map`, `key`
- Returns: none.
- Notes: missing keys are ignored.

### ck_hashmap_size

```c
size_t ck_hashmap_size(const ck_hashmap *map);
```

- Parameters: `map`
- Returns: current entry count.

### ck_hashmap_destroy

```c
void ck_hashmap_destroy(ck_hashmap *map);
```

- Parameters: `map`
- Returns: none.
- Notes: releases entries, buckets, and the opaque handle. Do not use `map` after this call.

## EXAMPLE

```c
#include <ckit/compare.h>
#include <ckit/datastruct/hashmap.h>
#include <stdint.h>

int main(void) {
    int status = 0;
    ck_hashmap *map;
    uint64_t key = 42;
    uint64_t value = 9001;
    uint64_t *found = NULL;

    map = ck_hashmap_create(sizeof(uint64_t), sizeof(uint64_t), ck_eq_u64, NULL);
    ck_hashmap_put(map, &key, &value);

    found = (uint64_t *)ck_hashmap_get(map, &key);
    if (found == NULL || *found != value) {
        status = 1;
        goto cleanup;
    }

    ck_hashmap_remove(map, &key);
    if (ck_hashmap_get(map, &key) != NULL) {
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_hashmap_destroy(map);
    return status;
}
```
