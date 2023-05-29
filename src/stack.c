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
