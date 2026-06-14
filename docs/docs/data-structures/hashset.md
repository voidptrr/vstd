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

### ck_hashset_create

```c
ck_hashset *ck_hashset_create(size_t elem_size,
                                ck_hashset_elem_eq_fn elem_eq,
                                ck_allocator *allocator);
```

- Parameters: `elem_size`, `elem_eq`, `allocator`
- Returns: opaque hashset handle.
- Notes: the hashset stores `allocator` and reuses it for entries, buckets,
  rehashing, and destroy. When `allocator` is `NULL`, hashset uses the C
  library heap through `ck_malloc`.

### ck_hashset_insert

```c
void ck_hashset_insert(ck_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: none.
- Notes: copies `elem` into set-managed storage when it is not already present.

### ck_hashset_contains

```c
bool ck_hashset_contains(const ck_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: `true` when an equal element exists, otherwise `false`.

### ck_hashset_get

```c
void *ck_hashset_get(ck_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: pointer to stored element in set-managed storage, or `NULL` when element is missing.

### ck_hashset_get_const

```c
const void *ck_hashset_get_const(const ck_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: const pointer to stored element in set-managed storage, or `NULL` when element is missing.

### ck_hashset_remove

```c
void ck_hashset_remove(ck_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: none.
- Notes: removes an equal element when present. Missing elements are ignored.

### ck_hashset_size

```c
size_t ck_hashset_size(const ck_hashset *set);
```

- Parameters: `set`
- Returns: current element count.

### ck_hashset_destroy

```c
void ck_hashset_destroy(ck_hashset *set);
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
    int status = 0;
    ck_hashset *set;
    uint64_t value = 42;
    const uint64_t *found = NULL;

    set = ck_hashset_create(sizeof(uint64_t), ck_eq_u64, NULL);
    ck_hashset_insert(set, &value);

    found = (const uint64_t *)ck_hashset_get_const(set, &value);
    if (found == NULL || *found != value) {
        status = 1;
        goto cleanup;
    }

    ck_hashset_remove(set, &value);
    if (ck_hashset_contains(set, &value) || ck_hashset_size(set) != 0) {
        status = 1;
        goto cleanup;
    }

cleanup:
    ck_hashset_destroy(set);
    return status;
}
```
