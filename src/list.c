/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#include <stdlib.h>
#include <stdbool.h>

#include "list.h"

void free_cell(CELL *cell, LIST *list);
CELL *alloc_cell(void);
void invalidate_cell(CELL *cell);

LIST *alloc_list(void) {
	LIST *list;

	list = malloc(sizeof(*list));
	list->first_cell = NULL;
	list->last_cell = NULL;
	list->iterators = 0;
	list->valid = 1;
	list->size = 0;
	return list;
}

void attach_iterator(ITERATOR *iterator, LIST *list) {
	iterator->list = list;

	if (list != NULL) {
		list->iterators++;
		iterator->cell = list->first_cell;
	} else
		iterator->cell = NULL;
}

CELL *alloc_cell(void) {
	CELL *cell;

	cell = malloc(sizeof(*cell));
	cell->next_cell = NULL;
	cell->prev_cell = NULL;
	cell->content = NULL;
	cell->valid = 1;
	return cell;
}

void attach_to_list(void *content, LIST *list) {
	CELL *cell;
	bool found = false;

	for (cell = list->first_cell; cell != NULL; cell = cell->next_cell) {
		if (!cell->valid) continue;

		if (cell->content == content) {
			found = true;
			break;
		}
	}

	if (found)
	return;

	cell = alloc_cell();
	cell->content = content;
	cell->next_cell = list->first_cell;

	if (list->first_cell != NULL)
		list->first_cell->prev_cell = cell;
	if (list->last_cell == NULL)
		list->last_cell = cell;

	list->first_cell = cell;
	list->size++;
}

void detach_from_list(void *content, LIST *list) {
	CELL *cell;

	for (cell = list->first_cell; cell != NULL; cell = cell->next_cell) {
		if (cell->content == content) {
			if (list->iterators > 0)
				invalidate_cell(cell);
			else
				free_cell(cell, list);

			list->size--;
			return;
		}
	}
}

void detach_iterator(ITERATOR *iterator) {
	LIST *list = iterator->list;

	if (list != NULL) {
		list->iterators--;

		if (list->iterators <= 0) {
			CELL *cell, *next_cell;

			for (cell = list->first_cell; cell != NULL; cell = next_cell) {
				next_cell = cell->next_cell;

				if (!cell->valid)
					free_cell(cell, list);
			}
			if (!list->valid)
				free_list(list);
		}
	}
}

void free_list(LIST *list) {
	CELL *cell, *next_cell;

	if (list->iterators > 0) {
		list->valid = 0;
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
	cell->valid = 0;
}

void *next_in_list(ITERATOR *iterator) {
	void *content = NULL;

	while (iterator->cell != NULL && !iterator->cell->valid) {
		iterator->cell = iterator->cell->next_cell;
	}

	if (iterator->cell != NULL) {
		content = iterator->cell->content;
		iterator->cell = iterator->cell->next_cell;
	}
	return content;
}

int list_size(LIST *list) {
	return list->size;
}
