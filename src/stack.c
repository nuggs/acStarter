/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#include <stdlib.h>

#include "stack.h"

typedef struct stack_cell STACK_CELL;

struct stack {
	STACK_CELL *cells;
	int size;
};

struct stack_cell {
	STACK_CELL *next;
	void *content;
};


STACK *alloc_stack() {
	STACK *stack;

	stack = malloc(sizeof(*stack));
	stack->cells = NULL;
	stack->size = 0;
	return stack;
}

void free_stack(STACK *stack) {
	STACK_CELL *cell;

	while ((cell = stack->cells) != NULL) {
		stack->cells = cell->next;
		free(cell);
	}
	free(stack);
}

void *pop_stack(STACK *stack)
{
	STACK_CELL *cell;

	if ((cell = stack->cells) != NULL) {
		void *content = cell->content;

		stack->cells = cell->next;
		stack->size--;
		free(cell);
		return content;
	}
	return NULL;
}

void push_stack(void *content, STACK *stack) {
	STACK_CELL *cell;

	cell = malloc(sizeof(*cell));
	cell->next = stack->cells;
	cell->content = content;
	stack->cells = cell;
	stack->size++;
}

int stack_size(STACK *stack) {
	return stack->size;
}
