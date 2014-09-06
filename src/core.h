/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */
#ifndef CORE_H
#define CORE_H

#include "list.h"

typedef enum {
	MODE_RACE,
	MODE_PRACTICE,
	MODE_DRIFT
} GAME_LOOP;

GAME_LOOP GAME_MODE; 
time_t current_time;
extern LIST *track_list;
extern LIST *entry_list;

void program_loop(int mode);

#endif
