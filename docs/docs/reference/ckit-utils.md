# ckit.utils

## DESCRIPTION

The utils module provides small public helpers shared by multiple APIs.

## MACROS

### CK_CONTAINER_OF

```c
#include <ckit/utils.h>

#define CK_CONTAINER_OF(ptr, type, member)
```

- Parameters: `ptr`, `type`, `member`
- Returns: pointer to the object that owns `member`.
- Notes: this is mainly useful with intrusive data structures, where an API returns a node pointer and the caller needs the containing object.

## EXAMPLE

```c
#include <ckit/datastruct/linked_list.h>
#include <ckit/utils.h>

typedef struct job {
    int id;
    ck_linked_list_node node;
} job;

int main(void) {
    ck_linked_list *list = ck_linked_list_init(NULL);
    job item = {.id = 7};

    ck_linked_list_push(list, &item.node);

    ck_linked_list_node *node = ck_linked_list_popleft(list);
    job *out = CK_CONTAINER_OF(node, job, node);

    ck_linked_list_deinit(list);
    return out->id == 7 ? 0 : 1;
}
```
