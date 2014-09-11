/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#ifndef _STACK_H
#define _STACK_H

typedef struct stack STACK;

STACK *alloc_stack(void);
void free_stack(STACK *stack);
void *pop_stack(STACK *stack);
void push_stack(void *content, STACK *stack);
int stack_size(STACK *stack);

#endif