
#include <stdlib.h>

#include "stack.h"

typedef struct scell stack_cell;

struct stack {
	stack_cell *cells;
	int size;
};

struct scell {
	stack_cell *next;
	void *content;
};

STACK *alloc_stack(void) {
	STACK *stack;

	stack = malloc(sizeof(*stack));
	stack->cells = NULL;
	stack->size = 0;

	return stack;
}

void free_stack(STACK *stack) {
	stack_cell *cell;

	while ((cell = stack->cells) != NULL) {
		stack->cells = cell->next;
		free(cell);
	}
	free(stack);
}

void *pop_stack(STACK *stack) {
	stack_cell *cell;

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
	stack_cell *cell;

	cell = malloc(sizeof(*cell));
	cell->next = stack->cells;
	cell->content = content;
	stack->cells = cell;
	stack->size++;
}

int stack_size(STACK *stack) {
	return stack->size;
}