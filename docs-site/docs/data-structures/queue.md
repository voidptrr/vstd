# cstd.datastruct.queue

## DESCRIPTION

The queue module provides a generic circular-buffer deque for fixed-size elements.
It supports front and back insertion/removal and read-only peeks on both ends.

## FUNCTIONS

### cstd_queue_init

```c
cstd_status cstd_queue_init(cstd_queue *queue, size_t elem_size);
```

- Parameters: `queue`, `elem_size`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `queue` is `NULL`; CSTD_ERR_RANGE if `elem_size == 0`.

### cstd_queue_push

```c
cstd_status cstd_queue_push(cstd_queue *queue, const void *element);
```

- Parameters: `queue`, `element`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `queue` or `element` is `NULL`.

### cstd_queue_pushfront

```c
cstd_status cstd_queue_pushfront(cstd_queue *queue, const void *element);
```

- Parameters: `queue`, `element`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `queue` or `element` is `NULL`.

### cstd_queue_popleft

```c
cstd_status cstd_queue_popleft(cstd_queue *queue, void *out);
```

- Parameters: `queue`, `out`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `queue` or `out` is `NULL`; CSTD_ERR_EMPTY if queue is empty.
- Notes: output parameter content is unspecified on failure.

### cstd_queue_popback

```c
cstd_status cstd_queue_popback(cstd_queue *queue, void *out);
```

- Parameters: `queue`, `out`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `queue` or `out` is `NULL`; CSTD_ERR_EMPTY if queue is empty.
- Notes: output parameter content is unspecified on failure.

### cstd_queue_peekleft

```c
cstd_status cstd_queue_peekleft(const cstd_queue *queue, void *out);
```

- Parameters: `queue`, `out`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `queue` or `out` is `NULL`; CSTD_ERR_EMPTY if queue is empty.
- Notes: output parameter content is unspecified on failure.

### cstd_queue_peekback

```c
cstd_status cstd_queue_peekback(const cstd_queue *queue, void *out);
```

- Parameters: `queue`, `out`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `queue` or `out` is `NULL`; CSTD_ERR_EMPTY if queue is empty.
- Notes: output parameter content is unspecified on failure.

### cstd_queue_free

```c
cstd_status cstd_queue_free(cstd_queue *queue);
```

- Parameters: `queue`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `queue` is `NULL`.

### cstd_queue_size

```c
size_t cstd_queue_size(const cstd_queue *queue);
```

- Parameters: `queue`
- Returns: current element count.
- Notes: returns `0` when `queue` is `NULL`.

### cstd_queue_is_empty

```c
bool cstd_queue_is_empty(const cstd_queue *queue);
```

- Parameters: `queue`
- Returns: `true` when empty; otherwise `false`.
- Notes: returns `true` when `queue` is `NULL`.

## EXAMPLE

```c
#include <cstd/datastruct/queue.h>
#include <cstd/status.h>
#include <stdint.h>

int main(void) {
    cstd_queue q;
    uint64_t first = 10;
    uint64_t second = 20;
    uint64_t out = 0;

    if (cstd_queue_init(&q, sizeof(uint64_t)) != CSTD_OK) {
        return 1;
    }
    cstd_queue_push(&q, &first);
    cstd_queue_pushfront(&q, &second);
    cstd_queue_peekleft(&q, &out);
    cstd_queue_popback(&q, &out);
    cstd_queue_free(&q);

    return 0;
}
```

## SEE ALSO

`cstd.status`, `cstd.datastruct`, `cstd.datastruct.benchmarks`
