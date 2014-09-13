/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
