/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
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

bool event_system_test(EVENT *event) {
	printf("Test 1: %s\n", ctime(&current_time));

	event = alloc_event();
	event->fun = &event_system_test;
	event->type = EVENT_SYSTEM_TEST;
	add_event_system(event, 10 * 60 * PULSES_PER_SECOND);
	return false;
}

/* 
 * Check every thirty seconds if the server is running,
 * if it's not, we restart it.
 * The ac_log will get created at each boot.
 */
bool event_system_checkac(EVENT *event) {
	char buf[256];
	pid_t pid = proc_find("acServer_linux");

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
	add_event_system(event, 30 * PULSES_PER_SECOND);
	return false;
}

bool event_track_raceover(EVENT *event) {
	TRACK *track;
	FILE *fp;
	pid_t pid = proc_find("acServer_linux");
	bool killed = false;
	char buf[4096]; /* Well, kick me in the dick if this isn't big enough */

	if ((track = event->owner.track) == NULL) {
		printf("event_track_test: No owner\n");
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
			printf("Found Raceover\n");
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
	add_event_track(event, track, 5 * PULSES_PER_SECOND);
	return false;
}

/* Just return true, no need to enqueue another event */
bool event_track_endpractice(EVENT *event) {
	TRACK *track;
	pid_t pid = proc_find("acServer_linux");

	if ((track = event->owner.track) == NULL) {
		printf("event_track_test: No owner\n");
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
	pid_t pid = proc_find("acServer_linux");
	bool killed = false;

	if ((track = event->owner.track) == NULL) {
		printf("event_track_test: No owner\n");
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
	add_event_track(event, track, 60 * PULSES_PER_SECOND);
	return false;
}

bool event_track_test(EVENT *event) {
	TRACK *track;

	if ((track = event->owner.track) == NULL) {
		printf("event_track_test: No owner\n");
		return true;
	}

	printf("Track %s: %s\n", track->track, ctime(&current_time));

	event = alloc_event();
	event->fun = &event_track_test;
	event->type = EVENT_TRACK_TEST;
	add_event_track(event, track, 60 * 60 * PULSES_PER_SECOND);
	return false;
}
