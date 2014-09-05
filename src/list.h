
#ifndef LIST_H
#define LIST_H

typedef struct cell {
	struct cell *next_cell;
	struct cell *prev_cell;
	void *content;
	int _valid;
} CELL;

typedef struct list {
	CELL *first_cell;
	CELL *last_cell;
	int _iterators;
	int _size;
	int _valid;
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
int size_of_list( LIST *list);

#endif
