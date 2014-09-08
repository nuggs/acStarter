
#ifndef _STACK_HEADER
#define _STACK_HEADER

typedef struct stack STACK;

STACK *alloc_stack(void);
void free_stack(STACK *stack);
void *pop_stack(STACK *stack);
void push_stack(void *content, STACK *stack);
int stack_size(STACK *stack);

#endif