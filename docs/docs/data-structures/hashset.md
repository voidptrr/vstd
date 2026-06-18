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

## FUNCTIONS

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

### vs_hashset_insert

```c
void vs_hashset_insert(vs_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: none.
- Notes: copies `elem` into set-managed storage when it is not already present.

### vs_hashset_contains

```c
bool vs_hashset_contains(const vs_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: `true` when an equal element exists, otherwise `false`.

### vs_hashset_get

```c
void *vs_hashset_get(vs_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: pointer to stored element in set-managed storage, or `NULL` when element is missing.

### vs_hashset_get_const

```c
const void *vs_hashset_get_const(const vs_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: const pointer to stored element in set-managed storage, or `NULL` when element is missing.

### vs_hashset_remove

```c
void vs_hashset_remove(vs_hashset *set, const void *elem);
```

- Parameters: `set`, `elem`
- Returns: none.
- Notes: removes an equal element when present. Missing elements are ignored.

### vs_hashset_size

```c
size_t vs_hashset_size(const vs_hashset *set);
```

- Parameters: `set`
- Returns: current element count.

### vs_hashset_destroy

```c
void vs_hashset_destroy(vs_hashset *set);
```

- Parameters: `set`
- Returns: none.
- Notes: releases entries, buckets, and the opaque handle. Do not use `set` after this call.

## EXAMPLE

```c
#include <vstd/datastruct/hashset.h>
#include <stdint.h>

int main(void) {
    int status = 0;
    vs_hashset *set;
    uint64_t value = 42;
    const uint64_t *found = NULL;

    set = vs_hashset_create(sizeof(uint64_t), NULL, NULL);
    vs_hashset_insert(set, &value);

    found = (const uint64_t *)vs_hashset_get_const(set, &value);
    if (found == NULL || *found != value) {
        status = 1;
        goto cleanup;
    }

    vs_hashset_remove(set, &value);
    if (vs_hashset_contains(set, &value) || vs_hashset_size(set) != 0) {
        status = 1;
        goto cleanup;
    }

cleanup:
    vs_hashset_destroy(set);
    return status;
}
```
