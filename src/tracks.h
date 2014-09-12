/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#ifndef TRACKS_H
#define TRACKS_H

extern TRACK *current_track;

typedef struct mode_data {
	bool enabled;
	char *name;
	int time;
	int wait_time;
} race_mode;

struct track_data {
	LIST *events;
	char *entry_list;
	char *name;
	char *cars;
	char *track;
	int sun_angle;
	int max_clients;
	int race_over_time;
	int port;
	int http_port;
	int register_to_lobby;
	int pickup_mode_enabled;
	int sleep_time;
	int voting_quorum;
	int vote_duration;
	int blacklist_mode;
	int client_send_interval_hz;
	int use_flow_control;
	int loop_mode;
	char *password;
	int allowed_tyres_out;
	int damage_multiplier;
	int fuel_rate;
	int tyre_wear_rate;
	race_mode practice;
	race_mode qualify;
	race_mode race;
	int dynamic_track[3];
	int track_number;
};

struct entry_data {
	char *drivername;
	char *team;
	char *model;
	char *skin;
	char *guid;
	int spectator_mode;
};

/* global file delcarations */
TRACK *alloc_track(void);
void free_track(TRACK *track);
int read_tracklist(const char *filename);
int write_track(void);
void next_track(void);

#endif
