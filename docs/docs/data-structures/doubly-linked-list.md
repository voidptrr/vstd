# datastruct.doubly_linked_list

## DESCRIPTION

The doubly_linked_list module provides an intrusive doubly linked list. User
objects embed `vs_doubly_linked_list_node`, and the list links those embedded
nodes directly.

The list owns only the list handle. Nodes and owning objects remain caller-owned.
Use `VS_CONTAINER_OF` to recover the owning object from a returned node.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### vs_doubly_linked_list_node

```c
typedef struct vs_doubly_linked_list_node {
    struct vs_doubly_linked_list_node *prev;
    struct vs_doubly_linked_list_node *next;
} vs_doubly_linked_list_node;
```

Embed this node in the object you want to link.

### VS_CONTAINER_OF

```c
#define VS_CONTAINER_OF(ptr, type, member)
```

- Parameters: `ptr`, `type`, `member`
- Returns: pointer to the owning object.

## FUNCTIONS

### vs_doubly_linked_list_create

```c
vs_doubly_linked_list *vs_doubly_linked_list_create(vs_allocator *allocator);
```

- Parameters: `allocator`
- Returns: opaque doubly linked-list handle.
- Notes: the list stores `allocator` and reuses it to destroy the list handle.
  Nodes remain caller-owned. When `allocator` is `NULL`, the handle uses the C
  library heap through `vs_malloc`.

### vs_doubly_linked_list_push

```c
void vs_doubly_linked_list_push(vs_doubly_linked_list *list,
                                  vs_doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: appends `node` at the tail.

### vs_doubly_linked_list_pushfront

```c
void vs_doubly_linked_list_pushfront(vs_doubly_linked_list *list,
                                       vs_doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: prepends `node` at the head.

### vs_doubly_linked_list_insert_after

```c
void vs_doubly_linked_list_insert_after(vs_doubly_linked_list *list,
                                          vs_doubly_linked_list_node *after,
                                          vs_doubly_linked_list_node *node);
```

- Parameters: `list`, `after`, `node`
- Returns: none.
- Notes: inserts `node` after `after`, or at the front when `after` is `NULL`.

### vs_doubly_linked_list_popleft

```c
vs_doubly_linked_list_node *vs_doubly_linked_list_popleft(vs_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: removed head node, or `NULL` when list is empty.

### vs_doubly_linked_list_popback

```c
vs_doubly_linked_list_node *vs_doubly_linked_list_popback(vs_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: removed tail node, or `NULL` when list is empty.

### vs_doubly_linked_list_remove

```c
void vs_doubly_linked_list_remove(vs_doubly_linked_list *list,
                                    vs_doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: unlinks a known node in `O(1)`.

### vs_doubly_linked_list_size

```c
size_t vs_doubly_linked_list_size(const vs_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current element count.

### vs_doubly_linked_list_head

```c
vs_doubly_linked_list_node *vs_doubly_linked_list_head(const vs_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current head node, or `NULL` when list is empty.

### vs_doubly_linked_list_tail

```c
vs_doubly_linked_list_node *vs_doubly_linked_list_tail(const vs_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current tail node, or `NULL` when list is empty.

### vs_doubly_linked_list_destroy

```c
void vs_doubly_linked_list_destroy(vs_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: none.
- Notes: releases only the opaque list handle. Nodes remain caller-owned. Do not use `list` after this call.

## EXAMPLE

```c
#include <vstd/datastruct/doubly_linked_list.h>
#include <vstd/memory/utils.h>

typedef struct job {
    int id;
    vs_doubly_linked_list_node node;
} job;

int main(void) {
    vs_doubly_linked_list *list = vs_doubly_linked_list_create(NULL);
    job first = {.id = 1};
    job second = {.id = 2};
    job third = {.id = 3};

    vs_doubly_linked_list_push(list, &first.node);
    vs_doubly_linked_list_push(list, &third.node);
    vs_doubly_linked_list_insert_after(list, &first.node, &second.node);

    vs_doubly_linked_list_remove(list, &second.node);

    vs_doubly_linked_list_node *node = vs_doubly_linked_list_popback(list);
    job *out = VS_CONTAINER_OF(node, job, node);

    vs_doubly_linked_list_destroy(list);
    return out->id == 3 ? 0 : 1;
}
```
