# memory.utils

## DESCRIPTION

The memory utils module provides low-level helpers for byte operations,
alignment, and intrusive data structures.

## MACROS

### CONTAINER_OF

```c
#include <vstd/memory/utils.h>

#define CONTAINER_OF(ptr, type, member)
```

- Parameters: `ptr`, `type`, `member`
- Returns: pointer to the object that owns `member`.
- Notes: this is mainly useful with intrusive data structures, where an API returns a node pointer and the caller needs the containing object.

### MEMORY_ALIGN

```c
#include <vstd/memory/utils.h>

#define MEMORY_ALIGN
```

- Returns: alignment used by vstd memory allocators for general-purpose storage.

## FUNCTIONS

### align_up

```c
#include <vstd/memory/utils.h>

static inline size_t align_up(size_t value, size_t alignment);
```

- Parameters: `value`, `alignment`
- Returns: `value` rounded up to the next multiple of `alignment`.
- Notes: `alignment` must be non-zero and a power of two.

### size_add_overflow

```c
#include <vstd/memory/utils.h>

static inline bool size_add_overflow(size_t lhs, size_t rhs, size_t *out);
```

- Parameters: `lhs`, `rhs`, `out`
- Returns: true when `lhs + rhs` would overflow.
- Writes: sum to `*out` when no overflow occurs.

### size_mul_overflow

```c
#include <vstd/memory/utils.h>

static inline bool size_mul_overflow(size_t lhs, size_t rhs, size_t *out);
```

- Parameters: `lhs`, `rhs`, `out`
- Returns: true when `lhs * rhs` would overflow.
- Writes: product to `*out` when no overflow occurs.

### align_up_overflow

```c
#include <vstd/memory/utils.h>

static inline bool align_up_overflow(size_t value, size_t alignment, size_t *out);
```

- Parameters: `value`, `alignment`, `out`
- Returns: true when rounding up would overflow.
- Writes: aligned value to `*out` when no overflow occurs.

### memswap

```c
#include <vstd/memory/utils.h>

void memswap(void *a, void *b, size_t size);
```

- Parameters: `a`, `b`, `size`
- Behavior: swaps `size` bytes between regions `a` and `b`.

## EXAMPLE

```c
#include <vstd/ds/linked_list.h>
#include <vstd/memory/utils.h>

typedef struct job {
    int id;
    linked_list_node node;
} job;

int main(void) {
    linked_list *list = NULL;
    if (linked_list_create(NULL, &list) != STATUS_OK) {
        /* handle allocation failure */
    }
    job item = {.id = 7};

    linked_list_push(list, &item.node);

    linked_list_node *node = linked_list_popleft(list);
    job *out = CONTAINER_OF(node, job, node);

    linked_list_destroy(list);
    return out->id == 7 ? 0 : 1;
}
```
