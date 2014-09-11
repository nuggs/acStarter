#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "core.h"
#include "config.h"
#include "tracks.h"

bool event_system_test(EVENT *event) {
	printf("Test 1: %s\n", ctime(&current_time));

	event = alloc_event();
	event->fun = &event_system_test;
	event->type = EVENT_SYSTEM_TEST;
	add_event_system(event, 10 * 60 * PULSES_PER_SECOND);
	return false;
}

bool event_system_checkac(EVENT *event) {
	char buf[128];
	pid_t pid = proc_find("acServer_linux");
	int status;

	if (pid == -1) {
		if (fork() == 0) {
			sprintf(buf, "%s%s -c=\"%s/cfg/server_cfg.ini\" -e=\"%s/cfg/entry_list.ini\" > ac_log", config->ac_location, config->ac_exe, config->ac_location,config->ac_location);
			status = system(buf);
			exit(0);
		}
	}

	event = alloc_event();
	event->fun = &event_system_checkac;
	event->type = EVENT_SYSTEM_CHECKAC;
	add_event_system(event, 30 * PULSES_PER_SECOND);
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
