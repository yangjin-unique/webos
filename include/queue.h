#include "list.h"

typedef struct list_head queue_t;


#define queue_init(h)   INIT_LIST_HEAD(h)

#define queue_insert(h, n)  list_add(n, h)

#define queue_insert_tail(h, n) list_add_tail(n, h)

#define queue_del(n) list_del(n)

#define queue_empty(h)  list_empty(h)

#define queue_entry(ptr, type, member) list_entry(ptr, type, member)

#define queue_head(h) (h)->next
