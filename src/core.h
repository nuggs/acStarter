/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */
#ifndef CORE_H
#define CORE_H

#include <stdbool.h>

#include "list.h"
#include "stack.h"

#define PULSES_PER_SECOND 4

typedef struct event_data EVENT;
typedef struct track_data TRACK;
typedef struct entry_data ENTRY;

typedef enum {
	MODE_RACE,
	MODE_PRACTICE,
	MODE_DRIFT
} GAME_LOOP;

GAME_LOOP GAME_MODE;
time_t current_time;

extern LIST *track_list;
extern LIST *entry_list;
extern LIST *global_events;
extern time_t current_time;

#include "event.h"

#endif
