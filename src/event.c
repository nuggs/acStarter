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

bool enqueue_event(struct event_data *event, int system_pulses) {
    int bucket, passes;

    if (event->owner_type == EVENT_UNOWNED) {
        ac_log(ERROR, "enqueue_event: event type %d with no owner\n", event->type);
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

void dequeue_event(struct event_data *event) {
    detach_from_list(event, event_queue[event->bucket]);

    switch(event->owner_type) {
        default:
            ac_log(ERROR, "dequeue_event: event type %d has no owner\n", event->type);
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

struct event_data *alloc_event(void) {
    struct event_data *event;

    if (stack_size(event_free) <= 0)
        event = malloc(sizeof(*event));
    else
        event = (struct event_data *) pop_stack(event_free);

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
    struct event_data *event;
    int i;

    if (section == 1) {
        for (i = 0; i < MAX_EVENT_HASH; i++) {
            event_queue[i] = alloc_list();
        }
        event_free = alloc_stack();
        global_events = alloc_list();
    } else if (section == 2) {
        event = alloc_event();
        event->fun = &event_system_checkac;
        event->type = EVENT_SYSTEM_CHECKAC;
        add_event_system(event, 10 * PPS);
    }
}

void init_events_track(struct track_data *track) {
    struct event_data *event;

    switch(game_mode) {
        case MODE_RACE:
            event = alloc_event();
            event->fun = &event_track_raceover;
            event->type = EVENT_TRACK_RACEOVER;
            add_event_track(event, track, 60 * PPS);

            /* For testing track cycling *
            event = alloc_event();
            event->fun = &event_track_nexttrack;
            event->type = EVENT_TRACK_NEXTTRACK;
            add_event_track(event, track, 30 * PPS);*/
        break;
        case MODE_PRACTICE:
            event = alloc_event();
            event->fun = &event_track_endpractice;
            event->type = EVENT_TRACK_ENDPRACTICE;
            add_event_track(event, track, track->practice.time * 60 * PPS);
        break;
        case MODE_DRIFT:
            /*
             * reusing the same event for this because why not?
             * drift is basically practice session with different cars
             */
            event = alloc_event();
            event->fun = &event_track_endpractice;
            event->type = EVENT_TRACK_ENDPRACTICE;
            add_event_track(event, track, track->practice.time * 60 * PPS);
        break;
        default:
            ac_log(ERROR, "init_events_track: Whoooa ho ho, we shouldn't be here\n");
        break;
    }
}

void heartbeat(void) {
    struct event_data *event;
    ITERATOR iterator;

    current_bucket = (current_bucket + 1) % MAX_EVENT_HASH;

    attach_iterator(&iterator, event_queue[current_bucket]);
    while ((event = (struct event_data *) next_in_list(&iterator)) != NULL) {
        if (event->passes-- > 0) continue;

        if (!((*event->fun)(event)))
            dequeue_event(event);
    }
    detach_iterator(&iterator);
}

void add_event_system(struct event_data *event, int delay) {
    if (event->type == EVENT_NONE) {
        ac_log(ERROR, "add_event_system: no type\n");
        return;
    }

    if (event->fun == NULL) {
        ac_log(ERROR, "add_event_system: event type %d has no callback function\n", event->type);
        return;
    }
    event->owner_type = EVENT_OWNER_SYSTEM;
    attach_to_list(event, global_events);

    if (enqueue_event(event, delay) == false)
        ac_log(ERROR, "add_event_system: event type %d failed to be enqueued\n", event->type);
}

void add_event_track(struct event_data *event, struct track_data *track, int delay) {
    if (event->type == EVENT_NONE) {
        ac_log(ERROR, "add_event_track: no type\n");
        return;
    }

    if (event->fun == NULL) {
        ac_log(ERROR, "add_event_track: event type %d has no callback function\n", event->type);
        return;
    }

    event->owner_type   = EVENT_OWNER_TRACK;
    event->owner.track = track;

    attach_to_list(event, track->events);

    if (enqueue_event(event, delay) == false)
        ac_log(ERROR, "add_event_track: event type %d failed to be enqueued\n", event->type);
}

struct event_data *event_isset_track(struct track_data *track, int type) {
    struct event_data *event;
    ITERATOR iterator;

    attach_iterator(&iterator, track->events);
    while ((event = (struct event_data *) next_in_list(&iterator)) != NULL) {
        if (event->type == type)
        break;
    }
    detach_iterator(&iterator);
    return event;
}

void strip_event_track(struct track_data *track, int type) {
    struct event_data *event;
    ITERATOR iterator;

    attach_iterator(&iterator, track->events);
    while ((event = (struct event_data *) next_in_list(&iterator)) != NULL) {
        if (event->type == type)
            dequeue_event(event);
    }
    detach_iterator(&iterator);
}

