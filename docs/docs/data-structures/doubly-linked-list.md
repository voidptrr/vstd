# datastruct.doubly_linked_list

## DESCRIPTION

The doubly_linked_list module provides an intrusive doubly linked list. User
objects embed `ck_doubly_linked_list_node`, and the list links those embedded
nodes directly.

The list owns only the list handle. Nodes and owning objects remain caller-owned.
Use `CK_CONTAINER_OF` to recover the owning object from a returned node.

This API is fail-fast: invalid required arguments are programmer errors and are asserted.

## TYPES

### ck_doubly_linked_list_node

```c
typedef struct ck_doubly_linked_list_node {
    struct ck_doubly_linked_list_node *prev;
    struct ck_doubly_linked_list_node *next;
} ck_doubly_linked_list_node;
```

Embed this node in the object you want to link.

### CK_CONTAINER_OF

```c
#define CK_CONTAINER_OF(ptr, type, member)
```

- Parameters: `ptr`, `type`, `member`
- Returns: pointer to the owning object.

## FUNCTIONS

### ck_doubly_linked_list_create

```c
ck_doubly_linked_list *ck_doubly_linked_list_create(ck_allocator *allocator);
```

- Parameters: `allocator`
- Returns: opaque doubly linked-list handle.
- Notes: the list stores `allocator` and reuses it to destroy the list handle.
  Nodes remain caller-owned. When `allocator` is `NULL`, the handle uses the C
  library heap through `ck_malloc`.

### ck_doubly_linked_list_push

```c
void ck_doubly_linked_list_push(ck_doubly_linked_list *list,
                                  ck_doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: appends `node` at the tail.

### ck_doubly_linked_list_pushfront

```c
void ck_doubly_linked_list_pushfront(ck_doubly_linked_list *list,
                                       ck_doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: prepends `node` at the head.

### ck_doubly_linked_list_insert_after

```c
void ck_doubly_linked_list_insert_after(ck_doubly_linked_list *list,
                                          ck_doubly_linked_list_node *after,
                                          ck_doubly_linked_list_node *node);
```

- Parameters: `list`, `after`, `node`
- Returns: none.
- Notes: inserts `node` after `after`, or at the front when `after` is `NULL`.

### ck_doubly_linked_list_popleft

```c
ck_doubly_linked_list_node *ck_doubly_linked_list_popleft(ck_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: removed head node, or `NULL` when list is empty.

### ck_doubly_linked_list_popback

```c
ck_doubly_linked_list_node *ck_doubly_linked_list_popback(ck_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: removed tail node, or `NULL` when list is empty.

### ck_doubly_linked_list_remove

```c
void ck_doubly_linked_list_remove(ck_doubly_linked_list *list,
                                    ck_doubly_linked_list_node *node);
```

- Parameters: `list`, `node`
- Returns: none.
- Notes: unlinks a known node in `O(1)`.

### ck_doubly_linked_list_size

```c
size_t ck_doubly_linked_list_size(const ck_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current element count.

### ck_doubly_linked_list_head

```c
ck_doubly_linked_list_node *ck_doubly_linked_list_head(const ck_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current head node, or `NULL` when list is empty.

### ck_doubly_linked_list_tail

```c
ck_doubly_linked_list_node *ck_doubly_linked_list_tail(const ck_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: current tail node, or `NULL` when list is empty.

### ck_doubly_linked_list_destroy

```c
void ck_doubly_linked_list_destroy(ck_doubly_linked_list *list);
```

- Parameters: `list`
- Returns: none.
- Notes: releases only the opaque list handle. Nodes remain caller-owned. Do not use `list` after this call.

## EXAMPLE

```c
#include <ckit/datastruct/doubly_linked_list.h>
#include <ckit/memory/utils.h>

typedef struct job {
    int id;
    ck_doubly_linked_list_node node;
} job;

int main(void) {
    ck_doubly_linked_list *list = ck_doubly_linked_list_create(NULL);
    job first = {.id = 1};
    job second = {.id = 2};
    job third = {.id = 3};

    ck_doubly_linked_list_push(list, &first.node);
    ck_doubly_linked_list_push(list, &third.node);
    ck_doubly_linked_list_insert_after(list, &first.node, &second.node);

    ck_doubly_linked_list_remove(list, &second.node);

    ck_doubly_linked_list_node *node = ck_doubly_linked_list_popback(list);
    job *out = CK_CONTAINER_OF(node, job, node);

    ck_doubly_linked_list_destroy(list);
    return out->id == 3 ? 0 : 1;
}
```
