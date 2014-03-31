#include "list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


void list_push(list_t **l, const void *elt, const uint8_t size) {
	if (l==NULL) {
		fprintf(stderr, "list_push error : invalid list reference.\n");
		return;
	} else if (elt == NULL) {
		fprintf(stderr, "list_push error : invalid element.\n");
		return;
	}

	void *elt_copy = malloc(size);
	memset(elt_copy, 0, size);
	memcpy(elt_copy, elt, size);

	list_t *temp = *l;
	(*l) = (list_t*)malloc(sizeof(list_t));
	(*l)->val = elt_copy;
	(*l)->next = temp;

	return;
}

//------------------------------------------------------------------------------------

void *list_pop(list_t **l) {
	if (l==NULL) {
		fprintf(stderr, "list_push error : invalid list reference.\n");
		return NULL;
	}

	if (*l == NULL) {
		fprintf(stderr, "list_pop warning : nothing to pop.\n");
		return NULL;
	}
	
	void *res = (*l)->val;
	list_t *temp = (*l)->next;
	free(*l);
	*l = temp;

	return res;
}

//------------------------------------------------------------------------------------

void list_destroy(list_t **l) {
	if (l==NULL) {
		fprintf(stderr, "list_destroy error : invalid list reference.\n");
		return;
	}

	while (*l != NULL) {
		list_t *temp = (*l)->next;
		free((*l)->val);
		free(*l);
		(*l) = temp;
	}

	*l = NULL;

	return;
}

//------------------------------------------------------------------------------------

char list_has_next(list_t *l) {
	return (l != NULL && (l->next != NULL));
}
