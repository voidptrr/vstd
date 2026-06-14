# datastruct.linked_list

## DESCRIPTION

The linked_list module provides an intrusive singly linked list. User objects embed
`ck_linked_list_node`, and the list links those embedded nodes directly.

The list owns only the list handle. Nodes and owning objects remain caller-owned.
Use `CK_CONTAINER_OF` to recover the owning object from a returned node.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### ck_linked_list_node

```c
typedef struct ck_linked_list_node {
    struct ck_linked_list_node *next;
} ck_linked_list_node;
```

Embed this node in the object you want to link.

### CK_CONTAINER_OF

```c
#define CK_CONTAINER_OF(ptr, type, member)
```

- Parameters: `ptr`, `type`, `member`
- Returns: pointer to the owning object.

## FUNCTIONS

### ck_linked_list_create

```c
ck_linked_list *ck_linked_list_create(ck_allocator *allocator);
```

- Parameters: `allocator`
- Returns: opaque linked-list handle.
- Notes: the list stores `allocator` and reuses it to destroy the list handle.
  Nodes remain caller-owned. When `allocator` is `NULL`, the handle uses the C
  library heap through `ck_malloc`.

### ck_linked_list_push

```c
void ck_linked_list_push(ck_linked_list *list, ck_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.

### ck_linked_list_pushfront

```c
void ck_linked_list_pushfront(ck_linked_list *list, ck_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.

### ck_linked_list_popleft

```c
ck_linked_list_node *ck_linked_list_popleft(ck_linked_list *list);
```

- Parameters: `list`
- Returns: removed head node, or `NULL` when list is empty.

### ck_linked_list_remove_after

```c
ck_linked_list_node *ck_linked_list_remove_after(ck_linked_list *list,
                                                     ck_linked_list_node *prev);
```

- Parameters: `list`, `prev`
- Returns: removed node after `prev`, or removed head node when `prev` is `NULL`.
- Notes: returns `NULL` when there is no node to remove.

### ck_linked_list_size

```c
size_t ck_linked_list_size(const ck_linked_list *list);
```

- Parameters: `list`
- Returns: current element count.

### ck_linked_list_head

```c
ck_linked_list_node *ck_linked_list_head(const ck_linked_list *list);
```

- Parameters: `list`
- Returns: current head node, or `NULL` when list is empty.

### ck_linked_list_destroy

```c
void ck_linked_list_destroy(ck_linked_list *list);
```

- Parameters: `list`
- Returns: none.
- Notes: releases only the opaque list handle. Nodes remain caller-owned. Do not use `list` after this call.

## EXAMPLE

```c
#include <ckit/datastruct/linked_list.h>
#include <ckit/utils.h>

typedef struct job {
    int id;
    ck_linked_list_node node;
} job;

int main(void) {
    ck_linked_list *list = ck_linked_list_create(NULL);
    job first = {.id = 1};
    job second = {.id = 2};

    ck_linked_list_push(list, &first.node);
    ck_linked_list_push(list, &second.node);

    ck_linked_list_node *node = ck_linked_list_popleft(list);
    job *out = CK_CONTAINER_OF(node, job, node);

    ck_linked_list_destroy(list);
    return out->id == 1 ? 0 : 1;
}
```
