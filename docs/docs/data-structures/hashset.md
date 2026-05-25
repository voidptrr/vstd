# datastruct.hashset

## DESCRIPTION

The hashset module provides fixed-size element storage with unique elements.
Hashing uses an internal FNV-1a function; element equality is provided by a
caller callback.

Elements are copied into set-managed storage. Duplicate inserts leave the set
unchanged. Collisions are resolved with bucket chains backed by intrusive linked
list nodes.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## FUNCTIONS

### ckit_hashset_init

```c
ckit_hashset *ckit_hashset_init(size_t elem_size,
                                ckit_hashset_elem_eq_fn elem_eq,
                                ckit_allocator *allocator);
```

- Parameters: `elem_size`, `elem_eq`, `allocator`
- Returns: opaque hashset handle.
- Notes: when `allocator` is `NULL`, hashset uses the C library heap through `ckit_malloc`.

### ckit_hashset_insert

```c
void ckit_hashset_insert(ckit_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: none.
- Notes: copies `elem` into set-managed storage when it is not already present.

### ckit_hashset_contains

```c
bool ckit_hashset_contains(const ckit_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: `true` when an equal element exists, otherwise `false`.

### ckit_hashset_get

```c
void *ckit_hashset_get(ckit_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: pointer to stored element in set-managed storage, or `NULL` when element is missing.

### ckit_hashset_get_const

```c
const void *ckit_hashset_get_const(const ckit_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: const pointer to stored element in set-managed storage, or `NULL` when element is missing.

### ckit_hashset_remove

```c
void ckit_hashset_remove(ckit_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: none.
- Notes: removes an equal element when present. Missing elements are ignored.

### ckit_hashset_size

```c
size_t ckit_hashset_size(const ckit_hashset *set);
```

- Parameters: `set`
- Returns: current element count.

### ckit_hashset_deinit

```c
void ckit_hashset_deinit(ckit_hashset *set);
```

- Parameters: `set`
- Returns: none.
- Notes: releases entries, buckets, and the opaque handle. Do not use `set` after this call.

## EXAMPLE

```c
#include <ckit/compare.h>
#include <ckit/datastruct/hashset.h>
#include <stdint.h>

int main(void) {
    ckit_hashset *set;
    uint64_t value = 42;
    const uint64_t *found = NULL;

    set = ckit_hashset_init(sizeof(uint64_t), ckit_eq_u64, NULL);
    ckit_hashset_insert(set, &value);

    found = (const uint64_t *)ckit_hashset_get_const(set, &value);
    if (found == NULL || *found != value) {
        ckit_hashset_deinit(set);
        return 1;
    }

    ckit_hashset_remove(set, &value);
    if (ckit_hashset_contains(set, &value) || ckit_hashset_size(set) != 0) {
        ckit_hashset_deinit(set);
        return 1;
    }

    ckit_hashset_deinit(set);
    return 0;
}
```
