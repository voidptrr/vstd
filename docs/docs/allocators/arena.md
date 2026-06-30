# memory.k4c_arena

## DESCRIPTION

The k4c_arena module provides a bump k4c_arena backed by an internal contiguous
region. Allocation moves an offset forward, and `k4c_arena_reset` releases all
k4c_arena allocations together by moving the offset back to zero.

Arena allocations are useful for short-lived groups of memory with the same
lifetime. Individual deallocation is unsupported, so the generic k4c_allocator view
does not advertise `K4C_ALLOCATOR_FEATURE_DEALLOC` and does not provide a k4c_dealloc
callback.

This API is fail-fast for required initialization/teardown preconditions.

## TYPES

### k4c_arena

```c
typedef struct k4c_arena k4c_arena;
```

- Notes: `k4c_arena` is opaque. Use the functions below to inspect or mutate
  k4c_arena state.

## FUNCTIONS

### k4c_arena_create

```c
k4c_status k4c_arena_create(size_t capacity, k4c_arena **out);
```

- Parameters: `capacity`, `out`
- Returns: `K4C_STATUS_OK` on success, or an error k4c_status.
- Writes: k4c_arena pointer to `*out` on success.
- Notes: capacity is aligned up to the k4c_arena's internal memory alignment.

### k4c_arena_allocator_view

```c
k4c_allocator k4c_arena_allocator_view(k4c_arena *k4c_arena);
```

- Parameters: `k4c_arena`
- Returns: generic k4c_allocator value for `k4c_arena`.
- Notes: the returned k4c_allocator advertises `K4C_ALLOCATOR_FEATURE_REALLOC | K4C_ALLOCATOR_FEATURE_RESET`.

### k4c_arena_reset

```c
void k4c_arena_reset(k4c_arena *k4c_arena);
```

- Parameters: `k4c_arena`
- Returns: none.
- Behavior: releases all k4c_arena allocations together while keeping the backing buffer.
- Notes: pointers returned before reset must not be used after reset.

### k4c_arena_capacity

```c
size_t k4c_arena_capacity(const k4c_arena *k4c_arena);
```

- Parameters: `k4c_arena`
- Returns: total managed bytes.

### k4c_arena_used

```c
size_t k4c_arena_used(const k4c_arena *k4c_arena);
```

- Parameters: `k4c_arena`
- Returns: number of bytes consumed from the k4c_arena buffer.

### k4c_arena_available

```c
size_t k4c_arena_available(const k4c_arena *k4c_arena);
```

- Parameters: `k4c_arena`
- Returns: number of bytes still available.

### k4c_arena_destroy

```c
void k4c_arena_destroy(k4c_arena *k4c_arena);
```

- Parameters: `k4c_arena`
- Returns: none.
- Behavior: releases the backing buffer and k4c_arena handle.

## EXAMPLES

### Allocate Scratch Memory

Use the allocator view when code should allocate through the generic allocator
API. Reset the arena when the whole scratch lifetime is finished.

```c
#include <stdint.h>

#include <k4c/error.h>
#include <k4c/allocators/allocator.h>
#include <k4c/allocators/arena.h>

int main(void) {
    k4c_arena *arena = NULL;
    if (k4c_arena_create(1024, &arena) != K4C_STATUS_OK) {
        return 1;
    }

    k4c_allocator allocator = k4c_arena_allocator_view(arena);
    uint64_t *value = NULL;
    if (k4c_alloc(&allocator, sizeof(uint64_t), (void **)&value) != K4C_STATUS_OK) {
        k4c_arena_destroy(arena);
        return 1;
    }

    *value = 42;

    k4c_arena_reset(arena);
    k4c_arena_destroy(arena);
    return 0;
}
```

### Inspect Usage

```c
#include <stddef.h>

#include <k4c/error.h>
#include <k4c/allocators/allocator.h>
#include <k4c/allocators/arena.h>

int main(void) {
    k4c_arena *arena = NULL;
    if (k4c_arena_create(2048, &arena) != K4C_STATUS_OK) {
        return 1;
    }

    k4c_allocator allocator = k4c_arena_allocator_view(arena);
    void *scratch = NULL;
    if (k4c_alloc(&allocator, 512, &scratch) != K4C_STATUS_OK) {
        k4c_arena_destroy(arena);
        return 1;
    }

    size_t capacity = k4c_arena_capacity(arena);
    size_t used = k4c_arena_used(arena);
    size_t available = k4c_arena_available(arena);

    if (used == 0 || available >= capacity) {
        k4c_arena_destroy(arena);
        return 1;
    }

    k4c_arena_destroy(arena);
    return 0;
}
```
