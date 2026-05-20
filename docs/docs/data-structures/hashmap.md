# datastruct.hashmap

## DESCRIPTION

The hashmap module provides fixed-size key/value mapping with separate chaining for collisions.
Hashing uses an internal FNV-1a function; key equality is provided by a caller callback.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## FUNCTIONS

### ckit_hashmap_init

```c
void ckit_hashmap_init(ckit_hashmap *map,
                       size_t key_size,
                       size_t value_size,
                       ckit_hashmap_key_eq_fn key_eq,
                       ckit_allocator *allocator);
```

- Parameters: `map`, `key_size`, `value_size`, `key_eq`, `allocator`
- Returns: none.
- Notes: when `allocator` is `NULL`, hashmap uses default allocator backing.

### ckit_hashmap_put

```c
void ckit_hashmap_put(ckit_hashmap *map, const void *key, const void *value);
```

- Parameters: `map`, `key`, `value`
- Returns: none.

### ckit_hashmap_get

```c
const void *ckit_hashmap_get(const ckit_hashmap *map, const void *key);
```

- Parameters: `map`, `key`
- Returns: pointer to stored value in map-managed storage, or `NULL` when key is missing.

### ckit_hashmap_remove

```c
void *ckit_hashmap_remove(ckit_hashmap *map, const void *key);
```

- Parameters: `map`, `key`
- Returns: pointer to removed value, or `NULL` when key is missing.

### ckit_hashmap_free

```c
void ckit_hashmap_free(ckit_hashmap *map);
```

- Parameters: `map`
- Returns: none.

### ckit_hashmap_size

```c
size_t ckit_hashmap_size(const ckit_hashmap *map);
```

- Parameters: `map`
- Returns: current entry count.
- Notes: returns `0` when `map` is `NULL`.

### ckit_hashmap_is_empty

```c
bool ckit_hashmap_is_empty(const ckit_hashmap *map);
```

- Parameters: `map`
- Returns: `true` when empty; otherwise `false`.
- Notes: returns `true` when `map` is `NULL`.

## EXAMPLE

```c
#include <ckit/compare.h>
#include <ckit/datastruct/hashmap.h>
#include <stdint.h>

int main(void) {
    ckit_hashmap map;
    uint64_t key = 42U;
    uint64_t value = 9001U;
    const uint64_t *found = NULL;
    uint64_t *removed = NULL;

    ckit_hashmap_init(&map, sizeof(uint64_t), sizeof(uint64_t), ckit_eq_u64, NULL);
    ckit_hashmap_put(&map, &key, &value);

    found = (const uint64_t *)ckit_hashmap_get(&map, &key);
    if (found == NULL || *found != value) {
        ckit_hashmap_free(&map);
        return 1;
    }

    removed = (uint64_t *)ckit_hashmap_remove(&map, &key);
    if (removed == NULL || *removed != value) {
        ckit_hashmap_free(&map);
        return 1;
    }

    ckit_hashmap_free(&map);
    return 0;
}
```
