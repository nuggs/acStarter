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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "core.h"
#include "config.h"
#include "tracks.h"
#include "cars.h"

/* local function declarations */
void system_loop(int mode);
void cleanup(void);

bool system_up = true;
bool quiet_mode = false;

static void signal_handler(int signo) {
	pid_t p;
	switch(signo) {
		case SIGINT:
			system_up = false;
		break;
		case SIGCHLD:
			do {
				p = waitpid(-1, NULL, WNOHANG);
			} while (p != (pid_t)0 && p != (pid_t)-1);
		break;
		case SIGTERM:
			system_up = false;
		break;
	}
}

void init_signals(void) {
	struct sigaction sigact;

	sigact.sa_handler = signal_handler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = SA_NOCLDSTOP | SA_RESTART | SA_SIGINFO | SA_NOCLDWAIT;
	sigaction(SIGINT, &sigact, (struct sigaction *)NULL);
	sigaction(SIGCHLD, &sigact, (struct sigaction *)NULL);
	sigaction(SIGTERM, &sigact, (struct sigaction *)NULL);
}

int main(int argc, char *argv[]) {
	char config_file[140+1], *home_dir = getenv("HOME");
	int next_option, use_drift = 0, use_race = 0, use_practice = 0;

	const char * usage = "Usage: %s [-h] [-d] [-p] [-r] [-q] [-v]\n"
			"  -h    --help               Display this usage information\n"
			"  -d    --drift              Read from drift configuration\n"
			"  -p    --practice           Read from practice configuration\n"
			"  -r    --race               Read from race configuration (\033[37;1mdefault\033[37;0m)\n"
			"  -q    --quiet              Don't display logging to console\n"
			"  -v    --version            Display version\n";

	const struct option long_options[] = {
		{ "help",       0, NULL,   'h' },
		{ "drift",      0, NULL,   'd' },
		{ "practice",   0, NULL,   'p' },
		{ "race",       0, NULL,   'r' },
		{ "quiet",      0, NULL,   'q' },
		{ "version",    0, NULL,   'v' },
		{ 0,            0, 0,       0  }
	};

	while((next_option = getopt_long(argc, argv, "hdprqv", long_options, NULL)) != -1) {
		switch(next_option) {
			case 'h':
				printf(usage, argv[0]);
				exit(EXIT_SUCCESS);
			break;

			case 'd':
				use_drift = 1;
			break;

			case 'p':
				use_practice = 1;
			break;

			case 'r':
				use_race = 1;
			break;

			case 'q':
				quiet_mode = true;
			break;

			case 'v':
				fprintf(stdout, "acStarter Version: %s\n"
								"Copyright (c) 2014 Turncoat Tony\n", VERSION);
				exit(EXIT_SUCCESS);
			break;

			default:
				printf(usage, argv[0]);
				exit(EXIT_FAILURE);
			break;
		}
	}

	init_signals();
	track_list = alloc_list();
	entry_list = alloc_list();
	skin_list = alloc_list();

	if ((use_race == 1 && use_practice == 1) || (use_race == 1 && use_drift == 1) || (use_practice == 1 && use_drift ==1)) {
		fprintf(stdout, "Please only select drift, practice or race\n");
		exit(EXIT_FAILURE);
	}

	if (use_drift == 1) {
		fprintf(stdout, "Starting with drift track list\n");
		snprintf(config_file, sizeof(config_file), "%s/acstarter/cfg/drift/drift.json", home_dir);
		GAME_MODE = MODE_DRIFT;
	} else if (use_practice == 1) {
		fprintf(stdout, "Starting with practice track list.\n");
		snprintf(config_file, sizeof(config_file), "%s/acstarter/cfg/practice/practice.json", home_dir);
		GAME_MODE = MODE_PRACTICE;
	} else {
		fprintf(stdout, "Starting with race track list.\n");
		snprintf(config_file, sizeof(config_file), "%s/acstarter/cfg/race/race.json", home_dir);
		GAME_MODE = MODE_RACE;
	}

	if (read_config(config_file) == -1) {
		fprintf(stdout, "Failed to read configuration file: %s\n", config_file);
		exit(EXIT_FAILURE);
	}

	if (read_tracklist(config->tracklist) == -1) {
		fprintf(stdout, "Failed to read configuration file: %s\n", config_file);
		exit(EXIT_FAILURE);
	}

	current_time = time(NULL);
	init_event_queue(1);
	init_dice();

	init_event_queue(2);

	system_loop(GAME_MODE);

	cleanup();
	ac_log(SYSLOG, "Program terminated successfully\n");
	return EXIT_SUCCESS;
}

void system_loop(int mode) {
	struct timeval last_time, new_time;
	long secs, usecs;

	if (current_track == NULL) {
		current_track = track_list->first_cell->content;
		write_track();
		init_events_track(current_track);
	}

	gettimeofday(&last_time, NULL);
	while (system_up) {
		current_time = time(NULL);

		heartbeat();

		gettimeofday(&new_time, NULL);

		/* NOTE: consider clock_nanosleep() */
		usecs = (int) (last_time.tv_usec -  new_time.tv_usec) + 1000000 / PPS;
		secs  = (int) (last_time.tv_sec  -  new_time.tv_sec);

		while (usecs < 0) {
			usecs += 1000000;
			secs  -= 1;
		}
		while (usecs >= 1000000) {
			usecs -= 1000000;
			secs  += 1;
		}

		if (secs > 0 || (secs == 0 && usecs > 0)) {
			struct timeval sleep_time;

			sleep_time.tv_usec = usecs;
			sleep_time.tv_sec  = secs;

			if (select(0, NULL, NULL, NULL, &sleep_time) < 0)
				continue;
		}
		gettimeofday(&last_time, NULL);
	}
}

void cleanup(void) {
	CAR_SKINS *skins;
	ITERATOR iterator;

	ac_log(SYSLOG, "cleaning up...\n");
	remove_server_config(REMOVE_CFG_BOTH);
	free_config();
	free_track(current_track);
	free_list(track_list);
	free_list(entry_list);

	attach_iterator(&iterator, skin_list);
	while ((skins = (CAR_SKINS *) next_in_list(&iterator)) != NULL)
		free_skin(skins);
	detach_iterator(&iterator);
	free_list(skin_list);
}
