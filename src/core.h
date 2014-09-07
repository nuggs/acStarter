/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */
#ifndef CORE_H
#define CORE_H

#include "list.h"
#include "stack.h"

/* 2, 4, 8 */
#define PASSES_PER_SECOND 8

typedef enum {
	MODE_RACE,
	MODE_PRACTICE,
	MODE_DRIFT
} GAME_LOOP;

/* I may switch to stdbool.h instead of defining it */
typedef enum {
	false = 0,
	true
} bool;

GAME_LOOP GAME_MODE; 
time_t current_time;
extern LIST *track_list;
extern LIST *entry_list;

void program_loop(int mode);

#endif
