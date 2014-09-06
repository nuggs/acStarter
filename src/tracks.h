
#ifndef TRACKS_H
#define TRACKS_H

typedef struct track_data TRACK;
typedef struct entry_data ENTRY;
extern TRACK *current_track;

typedef struct mode_data {
	char *name;
	int time;
	int wait_time;
} race_mode;

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
	race_mode practice;
	race_mode qualify;
	race_mode race;
	int dynamic_track[3];
};

struct entry_data {
	char *drivername;
	char *team;
	char *model;
	char *skin;
	int guid;
	int spectator_mode;
};

/* global file delcarations */
TRACK *alloc_track(void);
void free_track(TRACK *track);
int read_tracklist(const char *filename);
int write_track(void);

#endif
