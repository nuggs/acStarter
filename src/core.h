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
#ifndef CORE_H
#define CORE_H

#include <stdbool.h>

#include "list.h"
#include "stack.h"
#include "random.h"

#define SYSLOG 0
#define NETLOG 1
#define ERROR 2
#define DEBUG 3

#define PPS 4

enum game_loop {
    MODE_RACE,
    MODE_PRACTICE,
    MODE_DRIFT
};

extern enum game_loop game_mode;
extern time_t current_time;

extern LIST *track_list;
extern LIST *entry_list;
extern LIST *global_events;
extern LIST *skin_list;
extern time_t current_time;
extern bool quiet_mode;

#include "event.h"

void ac_log(int type, const char *txt, ...);

#endif
