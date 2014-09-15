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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#include "core.h"
#include "config.h"
#include "tracks.h"
#include "io.h"

/* 
 * Check every thirty seconds if the server is running,
 * if it's not, we restart it.
 * The ac_log will get created at each boot.
 */
bool event_system_checkac(EVENT *event) {
	char buf[256];
	pid_t pid = proc_find(config->ac_exe);

	if (pid == -1) {
		if (fork() == 0) {
			snprintf(buf, 256, "%s%s -c=\"%scfg/server_cfg.ini\" -e=\"%scfg/entry_list.ini\" > ac_log", config->ac_location, config->ac_exe, config->ac_location,config->ac_location);
			system(buf);
			exit(0);
		}
	}

	event = alloc_event();
	event->fun = &event_system_checkac;
	event->type = EVENT_SYSTEM_CHECKAC;
	add_event_system(event, 30 * PPS);
	return false;
}

bool event_track_raceover(EVENT *event) {
	TRACK *track;
	FILE *fp;
	pid_t pid = proc_find(config->ac_exe);
	bool killed = false;
	char buf[4096]; /* Well, kick me in the dick if this isn't big enough */

	if ((track = event->owner.track) == NULL) {
		ac_log(ERROR,"event_track_raceover: No owner\n");
		return true;
	}

	if ((fp = fopen("ac_log", "r")) == NULL) {
		return true;
	}

	while (fgets(buf, 4096, fp) != NULL) {
		if (strstr(buf, "HasSentRaceoverPacket")) {
			kill(pid, SIGTERM);
			killed = true;
			remove_server_config(REMOVE_CFG_BOTH);
		}
	}

	if (fp != NULL)
		fclose(fp);

	if (killed) {
		remove_file("ac_log");
		next_track();
		return true;
	}

	event = alloc_event();
	event->fun = &event_track_raceover;
	event->type = EVENT_TRACK_RACEOVER;
	add_event_track(event, track, 5 * PPS);
	return false;
}

/* Just return true, no need to enqueue another event */
bool event_track_endpractice(EVENT *event) {
	TRACK *track;
	pid_t pid = proc_find(config->ac_exe);

	if ((track = event->owner.track) == NULL) {
		ac_log(ERROR,"event_track_endpractice: No owner\n");
		return true;
	}

	kill(pid, SIGTERM);
	remove_server_config(REMOVE_CFG_BOTH);
	remove_file("ac_log");
	next_track();
	return true;
}

/* Used for track cycle testing */
bool event_track_nexttrack(EVENT *event) {
	TRACK *track;
	pid_t pid = proc_find(config->ac_exe);
	bool killed = false;

	if ((track = event->owner.track) == NULL) {
		ac_log(ERROR, "event_track_nexttrack: No owner\n");
		return true;
	}

	kill(pid, SIGTERM);
	killed = true;

	if (killed) {
		remove_server_config(REMOVE_CFG_BOTH);
		remove_file("ac_log");
		next_track();
		return true;
	}

	event = alloc_event();
	event->fun = &event_track_nexttrack;
	event->type = EVENT_TRACK_NEXTTRACK;
	add_event_track(event, track, 60 * PPS);
	return false;
}

