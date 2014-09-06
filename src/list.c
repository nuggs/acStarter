
#include <stdlib.h>

#include "list.h"

void free_cell(CELL *cell, LIST *list);
CELL *alloc_cell(void);
void invalidate_cell(CELL *cell);

LIST *alloc_list(void) {
	LIST *list;

	list = malloc(sizeof(*list));
	list->first_cell = NULL;
	list->last_cell = NULL;
	list->_iterators = 0;
	list->_valid = 1;
	list->_size = 0;

	return list;
}

void attach_iterator(ITERATOR *iterator, LIST *list) {
	iterator->list = list;

	if (list != NULL) {
		list->_iterators++;
		iterator->cell = list->first_cell;
	} else {
		iterator->cell = NULL;
	}
}

CELL *alloc_cell(void) {
	CELL *cell;

	cell		= malloc(sizeof(*cell));
	cell->next_cell	= NULL;
	cell->prev_cell = NULL;
	cell->content	= NULL;
	cell->_valid	= 1;

	return cell;
}

void attach_to_list(void *content, LIST *list) {
	CELL *cell;
	int found = 0;

	for (cell = list->first_cell; cell != NULL; cell = cell->next_cell) {
		if (!cell->_valid)
			continue;

		if (cell->content == content) {
			found = 1;
			break;
		}
	}

	if (found)
		return;

	cell = alloc_cell();
	cell->content = content;
	cell->next_cell	= list->first_cell;

	if (list->first_cell != NULL)
		list->first_cell->prev_cell = cell;

	if (list->last_cell == NULL)
		list->last_cell = cell;

	list->first_cell = cell;
	list->_size++;
}

void detach_from_list(void *content, LIST *list) {
	CELL *cell;

	for (cell = list->first_cell; cell != NULL; cell = cell->next_cell) {
		if (cell->content == content) {
			if (list->_iterators > 0)
				invalidate_cell(cell);
			else
				free_cell(cell, list);
			list->_size--;
			return;
		}
	}
}

void detach_iterator(ITERATOR *iterator) {
	LIST *list = iterator->list;

	if (list != NULL) {
		list->_iterators--;

		if (list->_iterators <= 0) {
			CELL *cell, *next_cell;

			for (cell = list->first_cell; cell != NULL; cell = next_cell) {
				next_cell = cell->next_cell;

				if (cell->_valid)
					free_cell(cell, list);
			}

			if (!list->_valid)
				free_list(list);
		}
	}
}

void free_list(LIST *list) {
	CELL *cell, *next_cell;

	if (list->_iterators > 0) {
		list->_valid = 0;
		return;
	}

	for (cell = list->first_cell; cell != NULL; cell = next_cell) {
		next_cell = cell->next_cell;
		free_cell(cell, list);
	}
	free(list);
}

void free_cell(CELL *cell, LIST *list) {
	if (list->first_cell == cell)
		list->first_cell = cell->next_cell;
	if (list->last_cell == cell)
		list->last_cell = cell->prev_cell;
	if (cell->prev_cell != NULL)
		cell->prev_cell->next_cell = cell->next_cell;
	if (cell->next_cell != NULL)
		cell->next_cell->prev_cell = cell->prev_cell;

	free(cell);
}

void invalidate_cell(CELL *cell) {
	cell->_valid = 0;
}

void *next_in_list(ITERATOR *iterator) {
	void *content = NULL;

	while (iterator->cell != NULL && !iterator->cell->_valid) {
		iterator->cell = iterator->cell->next_cell;
	}

	if (iterator->cell != NULL) {
		content = iterator->cell->content;
		iterator->cell = iterator->cell->next_cell;
	}

	return content;
}

int size_of_list(LIST *list) {
	return list->_size;
}
