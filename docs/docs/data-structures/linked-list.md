# datastruct.linked_list

## DESCRIPTION

The linked_list module provides an intrusive singly linked list. User objects embed
`ckit_linked_list_node`, and the list links those embedded nodes directly.

The list owns only the list handle. Nodes and owning objects remain caller-owned.
Use `ckit_container_of` to recover the owning object from a returned node.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### ckit_linked_list_node

```c
typedef struct ckit_linked_list_node {
    struct ckit_linked_list_node *next;
} ckit_linked_list_node;
```

Embed this node in the object you want to link.

### ckit_container_of

```c
#define ckit_container_of(ptr, type, member)
```

- Parameters: `ptr`, `type`, `member`
- Returns: pointer to the owning object.

## FUNCTIONS

### ckit_linked_list_init

```c
ckit_linked_list *ckit_linked_list_init(ckit_allocator *allocator);
```

- Parameters: `allocator`
- Returns: opaque linked-list handle.
- Notes: when `allocator` is `NULL`, the list handle uses the C library heap through `ckit_malloc`.

### ckit_linked_list_node_init

```c
void ckit_linked_list_node_init(ckit_linked_list_node *node);
```

- Parameters: `node`
- Returns: none.
- Notes: resets node linkage before first use or after removal.

### ckit_linked_list_push

```c
void ckit_linked_list_push(ckit_linked_list *list, ckit_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.

### ckit_linked_list_pushfront

```c
void ckit_linked_list_pushfront(ckit_linked_list *list, ckit_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.

### ckit_linked_list_popleft

```c
ckit_linked_list_node *ckit_linked_list_popleft(ckit_linked_list *list);
```

- Parameters: `list`
- Returns: removed head node, or `NULL` when list is empty.

### ckit_linked_list_free

```c
void ckit_linked_list_free(ckit_linked_list *list);
```

- Parameters: `list`
- Returns: none.
- Notes: releases only the opaque list handle. Nodes remain caller-owned. Do not use `list` after this call.

### ckit_linked_list_size

```c
size_t ckit_linked_list_size(const ckit_linked_list *list);
```

- Parameters: `list`
- Returns: current element count.
- Notes: returns `0` when `list` is `NULL`.

### ckit_linked_list_is_empty

```c
bool ckit_linked_list_is_empty(const ckit_linked_list *list);
```

- Parameters: `list`
- Returns: `true` when empty; otherwise `false`.
- Notes: returns `true` when `list` is `NULL`.

## EXAMPLE

```c
#include <ckit/datastruct/linked_list.h>

typedef struct job {
    int id;
    ckit_linked_list_node node;
} job;

int main(void) {
    ckit_linked_list *list = ckit_linked_list_init(NULL);
    job first = {.id = 1};
    job second = {.id = 2};

    ckit_linked_list_node_init(&first.node);
    ckit_linked_list_node_init(&second.node);

    ckit_linked_list_push(list, &first.node);
    ckit_linked_list_push(list, &second.node);

    ckit_linked_list_node *node = ckit_linked_list_popleft(list);
    job *out = ckit_container_of(node, job, node);

    ckit_linked_list_free(list);
    return out->id == 1 ? 0 : 1;
}
```
