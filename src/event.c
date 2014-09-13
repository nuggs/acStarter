/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "core.h"
#include "tracks.h"

LIST *event_queue[MAX_EVENT_HASH];
STACK *event_free = NULL;
LIST *global_events = NULL;
int current_bucket = 0;

bool enqueue_event(EVENT *event, int system_pulses) {
	int bucket, passes;

	if (event->owner_type == EVENT_UNOWNED) {
		printf("enqueue_event: event type %d with no owner\n", event->type);
		return false;
	}

	if (system_pulses < 1)
		system_pulses = 1;

	bucket = (system_pulses + current_bucket) % MAX_EVENT_HASH;
	passes = system_pulses / MAX_EVENT_HASH;

	event->passes = passes;
	event->bucket = bucket;

	attach_to_list(event, event_queue[bucket]);
	return true;
}

void dequeue_event(EVENT *event) {
	detach_from_list(event, event_queue[event->bucket]);

	switch(event->owner_type) {
		default:
			printf("dequeue_event: event type %d has no owner\n", event->type);
		break;
		case EVENT_OWNER_SYSTEM:
			detach_from_list(event, global_events);
		break;
		case EVENT_OWNER_TRACK:
			detach_from_list(event, event->owner.track->events);
		break;
	}
	free(event->argument);
	push_stack(event, event_free);
}

EVENT *alloc_event(void) {
	EVENT *event;

	if (stack_size(event_free) <= 0)
		event = malloc(sizeof(*event));
	else
		event = (EVENT *) pop_stack(event_free);

	event->fun = NULL;
	event->argument = NULL;
	event->owner.track = NULL;
	event->passes = 0;
	event->bucket = 0;
	event->owner_type = EVENT_UNOWNED;
	event->type = EVENT_NONE;
	return event;
}

void init_event_queue(int section) {
	EVENT *event;
	int i;

	if (section == 1) {
		for (i = 0; i < MAX_EVENT_HASH; i++) {
			event_queue[i] = alloc_list();
		}
		event_free = alloc_stack();
		global_events = alloc_list();
	} else if (section == 2) {
		event = alloc_event();
		event->fun = &event_system_test;
		event->type = EVENT_SYSTEM_TEST;
		add_event_system(event, 60 * PULSES_PER_SECOND);

		event = alloc_event();
		event->fun = &event_system_checkac;
		event->type = EVENT_SYSTEM_CHECKAC;
		add_event_system(event, 10 * PULSES_PER_SECOND);
	}
}

void init_events_track(TRACK *track)
{
	EVENT *event;

	event = alloc_event();
	event->fun = &event_track_test;
	event->type = EVENT_TRACK_TEST;
	add_event_track(event, track, 10 * 60 * PULSES_PER_SECOND);

	if (GAME_MODE == MODE_RACE) {
		event = alloc_event();
		event->fun = &event_track_raceover;
		event->type = EVENT_TRACK_RACEOVER;
		add_event_track(event, track, 60 * PULSES_PER_SECOND);

		/* For testing track cycling *
		event = alloc_event();
		event->fun = &event_track_nexttrack;
		event->type = EVENT_TRACK_NEXTTRACK;
		add_event_track(event, track, 30 * PULSES_PER_SECOND);*/
	}
}

void heartbeat(void) {
	EVENT *event;
	ITERATOR iterator;

	current_bucket = (current_bucket + 1) % MAX_EVENT_HASH;

	attach_iterator(&iterator, event_queue[current_bucket]);
	while ((event = (EVENT *) next_in_list(&iterator)) != NULL) {
		if (event->passes-- > 0) continue;

		if (!((*event->fun)(event)))
			dequeue_event(event);
	}
	detach_iterator(&iterator);
}

void add_event_system(EVENT *event, int delay) {
	if (event->type == EVENT_NONE) {
		printf("add_event_system: no type\n");
		return;
	}

	if (event->fun == NULL) {
		printf("add_event_system: event type %d has no callback function\n", event->type);
		return;
	}
	event->owner_type = EVENT_OWNER_SYSTEM;
	attach_to_list(event, global_events);

	if (enqueue_event(event, delay) == false)
		printf("add_event_system: event type %d failed to be enqueued\n", event->type);
}

void add_event_track(EVENT *event, TRACK *track, int delay) {
	if (event->type == EVENT_NONE) {
		printf("add_event_track: no type\n");
		return;
	}

	if (event->fun == NULL) {
		printf("add_event_track: event type %d has no callback function\n", event->type);
		return;
	}

	event->owner_type   = EVENT_OWNER_TRACK;
	event->owner.track = track;

	attach_to_list(event, track->events);

	if (enqueue_event(event, delay) == false)
		printf("add_event_track: event type %d failed to be enqueued\n", event->type);
}

EVENT *event_isset_track(TRACK *track, int type)
{
	EVENT *event;
	ITERATOR iterator;

	attach_iterator(&iterator, track->events);
	while ((event = (EVENT *) next_in_list(&iterator)) != NULL) {
		if (event->type == type)
		break;
	}
	detach_iterator(&iterator);
	return event;
}

void strip_event_track(TRACK *track, int type)
{
	EVENT *event;
	ITERATOR iterator;

	attach_iterator(&iterator, track->events);
	while ((event = (EVENT *) next_in_list(&iterator)) != NULL) {
		if (event->type == type)
			dequeue_event(event);
	}
	detach_iterator(&iterator);
}
