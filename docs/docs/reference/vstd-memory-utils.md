# memory.utils

## DESCRIPTION

The memory utils module provides low-level helpers for byte operations,
alignment, and intrusive data structures.

## MACROS

### VS_CONTAINER_OF

```c
#include <vstd/memory/utils.h>

#define VS_CONTAINER_OF(ptr, type, member)
```

- Parameters: `ptr`, `type`, `member`
- Returns: pointer to the object that owns `member`.
- Notes: this is mainly useful with intrusive data structures, where an API returns a node pointer and the caller needs the containing object.

### VS_MEMORY_ALIGN

```c
#include <vstd/memory/utils.h>

#define VS_MEMORY_ALIGN
```

- Returns: alignment used by vstd memory allocators for general-purpose storage.

## FUNCTIONS

### vs_align_up

```c
#include <vstd/memory/utils.h>

static inline size_t vs_align_up(size_t value, size_t alignment);
```

- Parameters: `value`, `alignment`
- Returns: `value` rounded up to the next multiple of `alignment`.
- Notes: `alignment` must be non-zero and a power of two.

### vs_size_add_overflow

```c
#include <vstd/memory/utils.h>

static inline bool vs_size_add_overflow(size_t lhs, size_t rhs, size_t *out);
```

- Parameters: `lhs`, `rhs`, `out`
- Returns: true when `lhs + rhs` would overflow.
- Writes: sum to `*out` when no overflow occurs.

### vs_size_mul_overflow

```c
#include <vstd/memory/utils.h>

static inline bool vs_size_mul_overflow(size_t lhs, size_t rhs, size_t *out);
```

- Parameters: `lhs`, `rhs`, `out`
- Returns: true when `lhs * rhs` would overflow.
- Writes: product to `*out` when no overflow occurs.

### vs_align_up_overflow

```c
#include <vstd/memory/utils.h>

static inline bool vs_align_up_overflow(size_t value, size_t alignment, size_t *out);
```

- Parameters: `value`, `alignment`, `out`
- Returns: true when rounding up would overflow.
- Writes: aligned value to `*out` when no overflow occurs.

### vs_memswap

```c
#include <vstd/memory/utils.h>

void vs_memswap(void *a, void *b, size_t size);
```

- Parameters: `a`, `b`, `size`
- Behavior: swaps `size` bytes between regions `a` and `b`.

## EXAMPLE

```c
#include <vstd/datastruct/linked_list.h>
#include <vstd/memory/utils.h>

typedef struct job {
    int id;
    vs_linked_list_node node;
} job;

int main(void) {
    vs_linked_list *list = NULL;
    if (vs_linked_list_create(NULL, &list) != VS_STATUS_OK) {
        /* handle allocation failure */
    }
    job item = {.id = 7};

    vs_linked_list_push(list, &item.node);

    vs_linked_list_node *node = vs_linked_list_popleft(list);
    job *out = VS_CONTAINER_OF(node, job, node);

    vs_linked_list_destroy(list);
    return out->id == 7 ? 0 : 1;
}
```
