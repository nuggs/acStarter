
#ifndef TRACKS_H
#define TRACKS_H

typedef struct track_data TRACK;

struct track_data {
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
	union {
		char *name;
		int time;
		int wait_time;
	} practice;
	union {
		char *name;
		int time;
		int wait_time;
	} qualify;
	union {
		char *name;
		int time;
		int wait_time;
	} race;
	int dynamic_track[3];
};

/* global file delcarations */
TRACK *alloc_track(void);
void free_track(TRACK *track);
int read_tracklist(const char *filename);

#endif