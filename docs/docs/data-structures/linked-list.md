# datastruct.linked_list

## DESCRIPTION

The linked_list module provides an intrusive singly linked list. User objects embed
`vs_linked_list_node`, and the list links those embedded nodes directly.

The list owns only the list handle. Nodes and owning objects remain caller-owned.
Use `VS_CONTAINER_OF` to recover the owning object from a returned node.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### vs_linked_list_node

```c
typedef struct vs_linked_list_node {
    struct vs_linked_list_node *next;
} vs_linked_list_node;
```

Embed this node in the object you want to link.

### VS_CONTAINER_OF

```c
#define VS_CONTAINER_OF(ptr, type, member)
```

- Parameters: `ptr`, `type`, `member`
- Returns: pointer to the owning object.

## FUNCTIONS

### vs_linked_list_create

```c
vs_linked_list *vs_linked_list_create(vs_allocator *allocator);
```

- Parameters: `allocator`
- Returns: opaque linked-list handle.
- Notes: the list stores `allocator` and reuses it to destroy the list handle.
  Nodes remain caller-owned. When `allocator` is `NULL`, the handle uses the C
  library heap through `vs_malloc`.

### vs_linked_list_push

```c
void vs_linked_list_push(vs_linked_list *list, vs_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.

### vs_linked_list_pushfront

```c
void vs_linked_list_pushfront(vs_linked_list *list, vs_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.

### vs_linked_list_popleft

```c
vs_linked_list_node *vs_linked_list_popleft(vs_linked_list *list);
```

- Parameters: `list`
- Returns: removed head node, or `NULL` when list is empty.

### vs_linked_list_remove_after

```c
vs_linked_list_node *vs_linked_list_remove_after(vs_linked_list *list,
                                                     vs_linked_list_node *prev);
```

- Parameters: `list`, `prev`
- Returns: removed node after `prev`, or removed head node when `prev` is `NULL`.
- Notes: returns `NULL` when there is no node to remove.

### vs_linked_list_size

```c
size_t vs_linked_list_size(const vs_linked_list *list);
```

- Parameters: `list`
- Returns: current element count.

### vs_linked_list_head

```c
vs_linked_list_node *vs_linked_list_head(const vs_linked_list *list);
```

- Parameters: `list`
- Returns: current head node, or `NULL` when list is empty.

### vs_linked_list_destroy

```c
void vs_linked_list_destroy(vs_linked_list *list);
```

- Parameters: `list`
- Returns: none.
- Notes: releases only the opaque list handle. Nodes remain caller-owned. Do not use `list` after this call.

## EXAMPLE

```c
#include <vstd/datastruct/linked_list.h>
#include <vstd/memory/utils.h>

typedef struct job {
    int id;
    vs_linked_list_node node;
} job;

int main(void) {
    vs_linked_list *list = vs_linked_list_create(NULL);
    job first = {.id = 1};
    job second = {.id = 2};

    vs_linked_list_push(list, &first.node);
    vs_linked_list_push(list, &second.node);

    vs_linked_list_node *node = vs_linked_list_popleft(list);
    job *out = VS_CONTAINER_OF(node, job, node);

    vs_linked_list_destroy(list);
    return out->id == 1 ? 0 : 1;
}
```
