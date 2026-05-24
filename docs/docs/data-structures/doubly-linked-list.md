# datastruct.doubly_linked_list

## DESCRIPTION

The doubly_linked_list module provides an intrusive doubly linked list. User
objects embed `ckit_doubly_linked_list_node`, and the list links those embedded
nodes directly.

The list owns only the list handle. Nodes and owning objects remain caller-owned.
Use `CKIT_CONTAINER_OF` to recover the owning object from a returned node.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### ckit_doubly_linked_list_node

```c
typedef struct ckit_doubly_linked_list_node {
    struct ckit_doubly_linked_list_node *prev;
    struct ckit_doubly_linked_list_node *next;
} ckit_doubly_linked_list_node;
```

Embed this node in the object you want to link.

### CKIT_CONTAINER_OF

```c
#define CKIT_CONTAINER_OF(ptr, type, member)
```

- Parameters: `ptr`, `type`, `member`
- Returns: pointer to the owning object.

## FUNCTIONS

### ckit_doubly_linked_list_init

```c
ckit_doubly_linked_list *ckit_doubly_linked_list_init(ckit_allocator *allocator);
```

- Parameters: `allocator`
- Returns: opaque doubly linked-list handle.
- Notes: when `allocator` is `NULL`, the list handle uses the C library heap through `ckit_malloc`.

### ckit_doubly_linked_list_push

```c
void ckit_doubly_linked_list_push(ckit_doubly_linked_list *list,
                                  ckit_doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: appends `node` at the tail.

### ckit_doubly_linked_list_pushfront

```c
void ckit_doubly_linked_list_pushfront(ckit_doubly_linked_list *list,
                                       ckit_doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: prepends `node` at the head.

### ckit_doubly_linked_list_insert_after

```c
void ckit_doubly_linked_list_insert_after(ckit_doubly_linked_list *list,
                                          ckit_doubly_linked_list_node *after,
                                          ckit_doubly_linked_list_node *node);
```

- Parameters: `list`, `after`, `node`
- Returns: none.
- Notes: inserts `node` after `after`, or at the front when `after` is `NULL`.

### ckit_doubly_linked_list_popleft

```c
ckit_doubly_linked_list_node *ckit_doubly_linked_list_popleft(ckit_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: removed head node, or `NULL` when list is empty.

### ckit_doubly_linked_list_popback

```c
ckit_doubly_linked_list_node *ckit_doubly_linked_list_popback(ckit_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: removed tail node, or `NULL` when list is empty.

### ckit_doubly_linked_list_remove

```c
void ckit_doubly_linked_list_remove(ckit_doubly_linked_list *list,
                                    ckit_doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: unlinks a known node in `O(1)`.

### ckit_doubly_linked_list_free

```c
void ckit_doubly_linked_list_free(ckit_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: none.
- Notes: releases only the opaque list handle. Nodes remain caller-owned. Do not use `list` after this call.

### ckit_doubly_linked_list_size

```c
size_t ckit_doubly_linked_list_size(const ckit_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current element count.

### ckit_doubly_linked_list_head

```c
ckit_doubly_linked_list_node *ckit_doubly_linked_list_head(const ckit_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current head node, or `NULL` when list is empty.

### ckit_doubly_linked_list_tail

```c
ckit_doubly_linked_list_node *ckit_doubly_linked_list_tail(const ckit_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current tail node, or `NULL` when list is empty.

## EXAMPLE

```c
#include <ckit/datastruct/doubly_linked_list.h>

typedef struct job {
    int id;
    ckit_doubly_linked_list_node node;
} job;

int main(void) {
    ckit_doubly_linked_list *list = ckit_doubly_linked_list_init(NULL);
    job first = {.id = 1};
    job second = {.id = 2};
    job third = {.id = 3};

    ckit_doubly_linked_list_push(list, &first.node);
    ckit_doubly_linked_list_push(list, &third.node);
    ckit_doubly_linked_list_insert_after(list, &first.node, &second.node);

    ckit_doubly_linked_list_remove(list, &second.node);

    ckit_doubly_linked_list_node *node = ckit_doubly_linked_list_popback(list);
    job *out = CKIT_CONTAINER_OF(node, job, node);

    ckit_doubly_linked_list_free(list);
    return out->id == 3 ? 0 : 1;
}
```
