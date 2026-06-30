# memory.utils

## DESCRIPTION

The memory utils module provides low-level helpers for byte operations,
alignment, and intrusive data structures.

## MACROS

### K4C_CONTAINER_OF

```c
#include <k4c/allocators/utils.h>

#define K4C_CONTAINER_OF(ptr, type, member)
```

- Parameters: `ptr`, `type`, `member`
- Returns: pointer to the object that owns `member`.
- Notes: this is mainly useful with intrusive data structures, where an API returns a node pointer and the caller needs the containing object.

### K4C_MEMORY_ALIGN

```c
#include <k4c/allocators/utils.h>

#define K4C_MEMORY_ALIGN
```

- Returns: alignment used by k4c memory allocators for general-purpose storage.

## FUNCTIONS

### k4c_memswap

```c
#include <k4c/allocators/utils.h>

void k4c_memswap(void *a, void *b, size_t size);
```

- Parameters: `a`, `b`, `size`
- Behavior: swaps `size` bytes between regions `a` and `b`.

## EXAMPLE

```c
#include <k4c/datastruct/linked_list.h>
#include <k4c/allocators/utils.h>

typedef struct job {
    int id;
    k4c_linked_list_node node;
} job;

int main(void) {
    k4c_linked_list *list = NULL;
    if (k4c_linked_list_create(NULL, &list) != K4C_STATUS_OK) {
        /* handle allocation failure */
    }
    job item = {.id = 7};

    k4c_linked_list_push(list, &item.node);

    k4c_linked_list_node *node = k4c_linked_list_popleft(list);
    job *out = K4C_CONTAINER_OF(node, job, node);

    k4c_linked_list_destroy(list);
    return out->id == 7 ? 0 : 1;
}
```
