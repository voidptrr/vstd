# ds.k4c_iterator

## DESCRIPTION

The k4c_iterator module provides a generic k4c_iterator that can walk all k4c data
structures with k4c_iterator support. Data structures own their k4c_iterator state,
construction, and advancement functions. The generic k4c_iterator module only owns
the basic callback k4c_iterator shape.

Iterators do not allocate. They borrow the data structure or caller-owned
callback context they point at.

The `context` parameter is intentionally generic. For `k4c_iterator_from_callback`
it is usually cursor state, such as an index into a k4c_vector. Pass `NULL` when a
callback does not need extra data.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### k4c_iterator

```c
typedef struct k4c_iterator k4c_iterator;
```

Iterator value. Store it by value and pass its address to `k4c_iterator_next`.

### k4c_iterator_next_fn

```c
typedef const void *(*k4c_iterator_next_fn)(void *context);
```

Callback used by custom iterators. Return the next item pointer, or `NULL` when
the k4c_iterator is exhausted.

## FUNCTIONS

### K4C_ITER_NEXT_AS

```c
#define K4C_ITER_NEXT_AS(type, iter)
```

- Parameters: `type`, `iter`
- Returns: typed pointer from `k4c_iterator_next`.
- Example:

```c
const int *item = K4C_ITER_NEXT_AS(int, &iter);
```

### k4c_iterator_for_each

```c
#define k4c_iterator_for_each(type, item, iter)
```

- Parameters: `type`, `item`, `iter`
- Notes: declares `const type *item` scoped to the loop body.
- Example:

```c
k4c_iterator_for_each(int, item, &iter) {
    /* use *item */
}
```

### k4c_iterator_from_callback

```c
k4c_iterator k4c_iterator_from_callback(void *context, k4c_iterator_next_fn next);
```

- Parameters: `context`, `next`
- Returns: k4c_iterator backed by caller-owned state.
- Notes: use this to integrate custom data structures with helpers that consume
  k4c_iterators.
- Example:

```c
typedef struct range_state {
    int current;
    int end;
} range_state;

static const void *range_next(void *context) {
    range_state *state = context;
    if (state->current >= state->end) {
        return NULL;
    }
    state->current += 1;
    return &state->current;
}

range_state state = {.current = 0, .end = 10};
k4c_iterator iter = k4c_iterator_from_callback(&state, range_next);
```

### k4c_iterator_next

```c
const void *k4c_iterator_next(k4c_iterator *iter);
```

- Parameters: `iter`
- Returns: next item pointer, or `NULL` when exhausted.
- Example:

```c
const int *item;
while ((item = (const int *)k4c_iterator_next(&iter)) != NULL) {
    /* use *item */
}
```

### k4c_iterator_set_size_hint

```c
void k4c_iterator_set_size_hint(k4c_iterator *iter, size_t size_hint);
```

- Parameters: `iter`, `size_hint`
- Returns: none.
- Notes: sets a conservative remaining-item count used by consumers to reserve
  output storage.

### k4c_iterator_size_hint

```c
size_t k4c_iterator_size_hint(const k4c_iterator *iter);
```

- Parameters: `iter`
- Returns: remaining-item hint, or zero when unknown.
