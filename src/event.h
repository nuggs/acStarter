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

#ifndef EVENT_H
#define EVENT_H

#define MAX_EVENT_HASH        128

#define EVENT_UNOWNED           0
#define EVENT_OWNER_NONE        1
#define EVENT_OWNER_SYSTEM      2
#define EVENT_OWNER_TRACK       3

#define EVENT_NONE              0

/* system events */
#define EVENT_SYSTEM_TEST       1
#define EVENT_SYSTEM_CHECKAC    2

/* track events */
#define EVENT_TRACK_TEST        1
#define EVENT_TRACK_RACEOVER    2
#define EVENT_TRACK_ENDPRACTICE 3
#define EVENT_TRACK_NEXTTRACK   4

typedef bool EVENT_FUN(EVENT *event);

struct event_data {
	EVENT_FUN *fun;
	char *argument;
	short int passes;
	short int type;
	short int owner_type;
	short int bucket;

	union {
		TRACK *track;
	} owner;
};

EVENT *alloc_event(void);
void dequeue_event(EVENT *event);
void init_event_queue(int section);
void init_events_track(TRACK *track);
void heartbeat(void);
void add_event_system(EVENT *event, int delay);
EVENT *event_isset_track(TRACK *track, int type);
void add_event_track(EVENT *event, TRACK *track, int delay);
void strip_event_track(TRACK *track, int type);

/* event callbacks */
bool event_system_test(EVENT *event);
bool event_system_checkac(EVENT *event);
bool event_track_test(EVENT *event);
bool event_track_raceover(EVENT *event);
bool event_track_endpractice(EVENT *event);
bool event_track_nexttrack(EVENT *event);

#endif
