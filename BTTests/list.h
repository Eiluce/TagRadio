#ifndef __LIST__
#define __LIST__

#include <stdint.h>
#include <string.h>

typedef struct list_t {
	void *val;
	struct list_t *next;
} list_t;

extern void list_push(list_t **l, const void *elt, const size_t size);
extern void *list_pop(list_t **l);
extern void *list_search(list_t ** l, const void *elt, const size_t size);
extern char list_has_next(list_t *l);
extern void list_destroy(list_t **l);

#endif
