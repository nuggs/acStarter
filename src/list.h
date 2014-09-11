/* file: list.h
 *
 * Headerfile for a basic double-linked list
 */

#ifndef _LIST_H
#define _LIST_H

typedef struct cell {
	struct cell *next_cell;
	struct cell *prev_cell;
	void *content;
	int valid;
} CELL;

typedef struct list {
	CELL *first_cell;
	CELL *last_cell;
	int iterators;
	int size;
	int valid;
} LIST;

typedef struct iterator {
  LIST *list;
  CELL *cell;
} ITERATOR;

LIST *alloc_list(void);
void attach_iterator(ITERATOR *iterator, LIST *list);
void *next_in_list(ITERATOR *iterator);
void attach_to_list(void *content, LIST *list);
void detach_from_list(void *content, LIST *list);
void detach_iterator(ITERATOR *iterator);
void free_list(LIST *list);
int list_size(LIST *list);

#endif
