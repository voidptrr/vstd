# cstd.datastruct.linked_list

## DESCRIPTION

The linked_list module provides a generic singly linked list for fixed-size elements.
It supports insertion at head and tail and removal from head.

## FUNCTIONS

### cstd_linked_list_init

```c
cstd_status cstd_linked_list_init(cstd_linked_list *list, size_t elem_size);
```

- Parameters: `list`, `elem_size`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `list` is `NULL`; CSTD_ERR_RANGE if `elem_size == 0`.

### cstd_linked_list_push

```c
cstd_status cstd_linked_list_push(cstd_linked_list *list, const void *element);
```

- Parameters: `list`, `element`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `list` or `element` is `NULL`; CSTD_ERR_OOM on allocation failure.

### cstd_linked_list_pushfront

```c
cstd_status cstd_linked_list_pushfront(cstd_linked_list *list, const void *element);
```

- Parameters: `list`, `element`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `list` or `element` is `NULL`; CSTD_ERR_OOM on allocation failure.

### cstd_linked_list_popleft

```c
cstd_status cstd_linked_list_popleft(cstd_linked_list *list, void *out);
```

- Parameters: `list`, `out`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `list` or `out` is `NULL`; CSTD_ERR_EMPTY if list is empty.
- Notes: output parameter content is unspecified on failure.

### cstd_linked_list_free

```c
cstd_status cstd_linked_list_free(cstd_linked_list *list);
```

- Parameters: `list`
- Returns: CSTD_OK on success.
- Errors: CSTD_ERR_NULL if `list` is `NULL`.

### cstd_linked_list_size

```c
size_t cstd_linked_list_size(const cstd_linked_list *list);
```

- Parameters: `list`
- Returns: current element count.
- Notes: returns `0` when `list` is `NULL`.

### cstd_linked_list_is_empty

```c
bool cstd_linked_list_is_empty(const cstd_linked_list *list);
```

- Parameters: `list`
- Returns: `true` when empty; otherwise `false`.
- Notes: returns `true` when `list` is `NULL`.

## EXAMPLE

```c
#include <cstd/datastruct/linked_list.h>
#include <cstd/status.h>
#include <stdint.h>

int main(void) {
    cstd_linked_list list;
    uint64_t a = 7;
    uint64_t b = 9;
    uint64_t out = 0;

    if (cstd_linked_list_init(&list, sizeof(uint64_t)) != CSTD_OK) {
        return 1;
    }
    cstd_linked_list_push(&list, &a);
    cstd_linked_list_pushfront(&list, &b);
    cstd_linked_list_popleft(&list, &out);
    cstd_linked_list_free(&list);

    return 0;
}
```

## SEE ALSO

`cstd.status`, `cstd.datastruct`, `cstd.datastruct.benchmarks`
