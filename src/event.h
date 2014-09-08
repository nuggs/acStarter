/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#ifndef EVENT_H
#define EVENT_H

#define MAX_EVENT_HASH 128

#define EVENT_UNOWNED 0
#define EVENT_OWNER_NONE 1
#define EVENT_OWNER_SYSTEM 2
#define EVENT_OWNER_TRACK 3

#define EVENT_NONE 0

/* system events */
#define EVENT_SYSTEM_TEST 1

/* Track events */
#define EVENT_TRACK_TEST 2

typedef bool EVENT_FUN(EVENT *event);

struct event_data {
  EVENT_FUN *function;
  char *argument;
  short int passes;
  short int type;
  short int owner_type;
  short int bucket;

  union {
    TRACK *track;   /* We can add more owner types later if */
  } owner;                    /* required this will do for now */
};                           

EVENT *alloc_event(void);
EVENT *event_isset_track(TRACK *track, int type);
void dequeue_event(EVENT *event);
void init_event_queue(int section);
void init_events_track(TRACK *track);
void heartbeat(void);
void add_event_track(EVENT *event, TRACK *track, int delay);
void add_event_system(EVENT *event, int delay);
void strip_event_track(TRACK *track, int type);

/* event callbacks */
bool event_system_test(EVENT *event);
bool event_track_test(EVENT *event);

#endif
