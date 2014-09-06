/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

#include "core.h"
#include "config.h"
#include "io.h"
#include "tracks.h"

int running = -1;

/* local function declarations */
static void signal_handler(int signo);
void init_signals(void);
void handle_race(void);

int main(int argc, char* argv[]) {
	char config_file[140+1], *home_dir = getenv("HOME");
	int next_option, use_drift = 0, use_race = 0, use_practice = 0;

	const char * usage = "Usage: %s [-h] [-d] [-p] [-r] [-v]\n"
			"  -h    --help               Display this usage information.\n"
			"  -d    --drift              Read from drift configuration.\n"
			"  -p    --practice           Read from practice configuration.\n"
			"  -r    --race               Read from race configuration (\033[37;1mdefault\033[37;0m).\n"
			"  -v    --version            Display version number.\n";

	const struct option long_options[] = {
		{ "help",       0, NULL,   'h' },
		{ "drift",      0, NULL,   'd' },
		{ "practice",   0, NULL,   'p' },
		{ "race",       0, NULL,   'r' },
		{ "version",    0, NULL,   'v' },
		{ 0,            0, 0,       0  }
	};

	while((next_option = getopt_long(argc, argv, "hdprv", long_options, NULL)) != -1) {
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

	/* allocate memory for the lists */
	track_list = alloc_list();
	entry_list = alloc_list();

	/* initialize signal handler */
	init_signals();

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

	running = 1;
	program_loop(GAME_MODE);

	free_config();
	free_list(track_list);

	fprintf(stdout, "\nProgram exited normally\n");
	return EXIT_SUCCESS;
}

void program_loop(int mode) {
	struct timeval last_time, new_time;
	long secs, usecs;

	gettimeofday(&last_time, NULL);
	while (running) {
		gettimeofday(&new_time, NULL);

		switch (GAME_MODE) {
			case MODE_RACE:
				handle_race();
			break;

			case MODE_PRACTICE:
				fprintf(stdout, "Looping practice mode\n");
			break;

			case MODE_DRIFT:
				fprintf(stdout, "Looping Drift mode\n");
			break;

			default:
				fprintf(stdout, "We shouldn't be looping here...");
				running = 0;
			break;
		}

		usecs = (int) (last_time.tv_usec -  new_time.tv_usec) + 1000000 / 8;
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

void init_signals(void) {
	struct sigaction sigact;

	sigact.sa_handler = signal_handler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, (struct sigaction *)NULL);
}

static void signal_handler(int signo) {
	if (signo == SIGINT)
		running = 0;
}

void handle_race(void) {
	ITERATOR iterator;
	TRACK *track = NULL;

	if (current_track == NULL) {
		current_track = track_list->first_cell->content;
		write_track();
	}

	printf("Testing: %s\n", current_track->track);
}