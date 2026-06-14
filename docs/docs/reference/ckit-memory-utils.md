# memory.utils

## DESCRIPTION

The memory utils module provides low-level helpers for byte operations,
alignment, and intrusive data structures.

## MACROS

### CK_CONTAINER_OF

```c
#include <ckit/memory/utils.h>

#define CK_CONTAINER_OF(ptr, type, member)
```

- Parameters: `ptr`, `type`, `member`
- Returns: pointer to the object that owns `member`.
- Notes: this is mainly useful with intrusive data structures, where an API returns a node pointer and the caller needs the containing object.

### CK_MEMORY_ALIGN

```c
#include <ckit/memory/utils.h>

#define CK_MEMORY_ALIGN
```

- Returns: alignment used by ckit memory allocators for general-purpose storage.

## FUNCTIONS

### ck_align_up

```c
#include <ckit/memory/utils.h>

static inline size_t ck_align_up(size_t value, size_t alignment);
```

- Parameters: `value`, `alignment`
- Returns: `value` rounded up to the next multiple of `alignment`.
- Notes: `alignment` must be non-zero and a power of two.

### ck_memswap

```c
#include <ckit/memory/utils.h>

void ck_memswap(void *a, void *b, size_t size);
```

- Parameters: `a`, `b`, `size`
- Behavior: swaps `size` bytes between regions `a` and `b`.

## EXAMPLE

```c
#include <ckit/datastruct/linked_list.h>
#include <ckit/memory/utils.h>

typedef struct job {
    int id;
    ck_linked_list_node node;
} job;

int main(void) {
    ck_linked_list *list = ck_linked_list_create(NULL);
    job item = {.id = 7};

    ck_linked_list_push(list, &item.node);

    ck_linked_list_node *node = ck_linked_list_popleft(list);
    job *out = CK_CONTAINER_OF(node, job, node);

    ck_linked_list_destroy(list);
    return out->id == 7 ? 0 : 1;
}
```
