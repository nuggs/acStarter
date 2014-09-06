/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "config.h"
#include "tracks.h"
#include "parson.h"

LIST *track_list = NULL;

/* local function delclarations */
int parse_track(JSON_Object *track);
typedef enum {
	TYPE_INT,
	TYPE_ARRAY,
	TYPE_STRING
} TRACK_VAL_TYPE;

TRACK *alloc_track(void) {
	TRACK *track;
	int i;

	track							= malloc(sizeof(*track));
	track->name						= NULL;
	track->cars						= NULL;
	track->track					= NULL;
	track->sun_angle				= 0;
	track->max_clients				= 0;
	track->race_over_time			= 0;
	track->port						= 0;
	track->http_port				= 0;
	track->register_to_lobby		= 0;
	track->pickup_mode_enabled		= 0;
	track->sleep_time				= 0;
	track->voting_quorum			= 0;
	track->vote_duration			= 0;
	track->blacklist_mode			= 0;
	track->client_send_interval_hz	= 0;
	track->use_flow_control			= 0;
	track->loop_mode				= 0;
	track->password					= NULL;
	track->allowed_tyres_out		= 0;
	track->damage_multiplier		= 0;
	track->fuel_rate				= 0;
	track->tyre_wear_rate			= 0;
	track->practice.name			= NULL;
	track->practice.time			= 0;
	track->practice.wait_time		= 0;
	track->qualify.name				= NULL;
	track->qualify.time				= 0;
	track->qualify.wait_time		= 0;
	track->race.name				= NULL;
	track->race.time				= 0;
	track->race.wait_time			= 0;
	for (i=0;i<=3;i++)
		track->dynamic_track[i]		= 0;

	return track;
}

void free_track(TRACK *track) {
	free(track->name);
	free(track->cars);
	free(track->track);
	free(track->password);
	free(track->practice.name);
	free(track->qualify.name);
	free(track->race.name);
	free(track);
	return;
}

int read_tracklist(const char *filename) {
	JSON_Value *tracks_root;
	JSON_Array *tracks_array;
	JSON_Object *tracks_object;
	int i;

	tracks_root = json_parse_file_with_comments(filename);
	if (json_value_get_type(tracks_root) != JSONArray) {
		fprintf(stdout, "JSON is not an array\n");
		return -1;
	}

	tracks_array = json_value_get_array(tracks_root);
	for (i=0;i<json_array_get_count(tracks_array);i++) {
		tracks_object = json_array_get_object(tracks_array, i);
		parse_track(tracks_object);
	}

	json_value_free(tracks_root);
	return 0;
}

int parse_track(JSON_Object *track_data) {
	TRACK *track;

	if (json_object_get_string(track_data, "track") == NULL) {
		if (config->defaults != NULL) {
			printf("Defaults already populated, config has missing or null \"track\" key\n");
			return -1;
		}
		fprintf(stdout, "Loading track defaults\n");
		config->defaults							= alloc_track();
		config->defaults->name						= strdup(json_object_get_string(track_data, "name"));
		config->defaults->cars						= strdup(json_object_get_string(track_data, "cars"));
		config->defaults->track						= NULL;
		config->defaults->sun_angle					= json_object_get_number(track_data, "sun_angle");
		config->defaults->max_clients				= json_object_get_number(track_data, "max_clients");
		config->defaults->race_over_time			= json_object_get_number(track_data, "race_over_time");
		config->defaults->port						= json_object_get_number(track_data, "port");
		config->defaults->http_port					= json_object_get_number(track_data, "http_port");
		config->defaults->register_to_lobby			= json_object_get_number(track_data, "register_to_lobby");
		config->defaults->pickup_mode_enabled		= json_object_get_number(track_data, "pickup_mode_enabled");
		config->defaults->sleep_time				= json_object_get_number(track_data, "sleep_time");
		config->defaults->voting_quorum				= json_object_get_number(track_data, "voting_quorum");
		config->defaults->vote_duration				= json_object_get_number(track_data, "vote_duration");
		config->defaults->blacklist_mode			= json_object_get_number(track_data, "blacklist_mode");
		config->defaults->client_send_interval_hz	= json_object_get_number(track_data, "client_send_interval_hz");
		config->defaults->use_flow_control			= json_object_get_number(track_data, "use_flow_control");
		config->defaults->loop_mode					= json_object_get_number(track_data, "loop_mode");
		if (json_object_get_string(track_data, "password") != NULL)
			config->defaults->password				= strdup(json_object_get_string(track_data, "password"));
		config->defaults->allowed_tyres_out			= json_object_get_number(track_data, "allowed_tyres_out");
		config->defaults->damage_multiplier			= json_object_get_number(track_data, "damage_multiplier");
		config->defaults->fuel_rate					= json_object_get_number(track_data, "fuel_rate");
		config->defaults->tyre_wear_rate			= json_object_get_number(track_data, "tyre_wear_rate");
		if (json_object_dotget_boolean(track_data, "practice.enabled") == 1) {
			config->defaults->practice.name			= strdup(json_object_dotget_string(track_data, "practice.name"));
			config->defaults->practice.time			= json_object_dotget_number(track_data, "practice.time");
			config->defaults->practice.wait_time	= json_object_dotget_number(track_data, "practice.wait_time");
		}
		if (json_object_dotget_boolean(track_data, "qualify.enabled") == 1) {
			config->defaults->qualify.name			= strdup(json_object_dotget_string(track_data, "qualify.name"));
			config->defaults->qualify.time			= json_object_dotget_number(track_data, "qualify.time");
			config->defaults->qualify.wait_time		= json_object_dotget_number(track_data, "qualify.wait_time");
		}
		if (json_object_dotget_boolean(track_data, "race.enabled") == 1) {
			config->defaults->race.name				= strdup(json_object_dotget_string(track_data, "race.name"));
			config->defaults->race.time				= json_object_dotget_number(track_data, "race.time");
			config->defaults->race.wait_time		= json_object_dotget_number(track_data, "race.wait_time");
		}
		config->defaults->dynamic_track[0] 			= json_object_dotget_number(track_data, "dynamic_track.session_start");
		config->defaults->dynamic_track[1] 			= json_object_dotget_number(track_data, "dynamic_track.randomness");
		config->defaults->dynamic_track[2] 			= json_object_dotget_number(track_data, "dynamic_track.lap_gain");
		config->defaults->dynamic_track[3] 			= json_object_dotget_number(track_data, "dynamic_track.session_transfer");
		fprintf(stdout, "Practice Details: %s\n", config->defaults->race.name);

		return 1;
	}
	
	fprintf(stdout, "Loading track: %s (%g pits)\n", json_object_get_string(track_data, "track"), json_object_get_number(track_data, "max_clients"));
	if (json_object_get_string(track_data, "track") == NULL) {
		printf("You must specify a track, there is no default track\nTrack not loaded, loading next track\n");
		return -1;
	}
	track							= alloc_track();
	track->name						= (json_object_get_string(track_data, "name") != NULL) ? strdup(json_object_get_string(track_data, "name")) : strdup(config->defaults->name);
	track->cars						= (json_object_get_string(track_data, "cars") != NULL) ? strdup(json_object_get_string(track_data, "cars")) : strdup(config->defaults->cars);
	track->track					= strdup(json_object_get_string(track_data, "track"));
	track->sun_angle				= (json_object_get_number(track_data, "sun_angle") != 0) ? json_object_get_number(track_data, "sun_angle") : config->defaults->sun_angle;
	track->max_clients				= (json_object_get_number(track_data, "max_clients") != 0) ? json_object_get_number(track_data, "max_clients") : config->defaults->max_clients;
	track->race_over_time			= (json_object_get_number(track_data, "race_over_time") != 0) ? json_object_get_number(track_data, "race_over_time") : config->defaults->race_over_time;
	track->port						= (json_object_get_number(track_data, "port") != 0) ? json_object_get_number(track_data, "port") : config->defaults->port;
	track->http_port				= (json_object_get_number(track_data, "http_port") != 0) ? json_object_get_number(track_data, "http_port") : config->defaults->http_port;
	track->register_to_lobby		= (json_object_get_number(track_data, "register_to_lobby") != 0) ? json_object_get_number(track_data, "register_to_lobby") : config->defaults->register_to_lobby;
	track->pickup_mode_enabled		= (json_object_get_number(track_data, "pickup_mode_enabled") != 0) ? json_object_get_number(track_data, "pickup_mode_enabled") : config->defaults->pickup_mode_enabled;
	track->sleep_time				= (json_object_get_number(track_data, "sleep_time") != 0) ? json_object_get_number(track_data, "sleep_time") : config->defaults->sleep_time;
	track->voting_quorum			= (json_object_get_number(track_data, "voting_quorum") != 0) ? json_object_get_number(track_data, "voting_quorum") : config->defaults->voting_quorum;
	track->vote_duration			= (json_object_get_number(track_data, "vote_duration") != 0) ? json_object_get_number(track_data, "vote_duration") : config->defaults->vote_duration;
	track->blacklist_mode			= (json_object_get_number(track_data, "blacklist_mode") != 0) ? json_object_get_number(track_data, "blacklist_mode") : config->defaults->blacklist_mode;
	track->client_send_interval_hz	= (json_object_get_number(track_data, "client_send_interval_hz") != 0) ? json_object_get_number(track_data, "client_send_interval_hz") : config->defaults->client_send_interval_hz;
	track->use_flow_control			= (json_object_get_number(track_data, "use_flow_control") != 0) ? json_object_get_number(track_data, "use_flow_control") : config->defaults->use_flow_control;
	track->loop_mode				= (json_object_get_number(track_data, "loop_mode") != 0) ? json_object_get_number(track_data, "loop_mode") : config->defaults->loop_mode;
	if (json_object_get_string(track_data, "password") != NULL)
		track->password				= strdup(json_object_get_string(track_data, "password"));
	track->allowed_tyres_out		= (json_object_get_number(track_data, "allowed_tyres_out") != 0) ? json_object_get_number(track_data, "allowed_tyres_out") : config->defaults->allowed_tyres_out;
	track->damage_multiplier		= (json_object_get_number(track_data, "damage_multiplier") != 0) ? json_object_get_number(track_data, "damage_multiplier") : config->defaults->damage_multiplier;
	track->fuel_rate				= (json_object_get_number(track_data, "fuel_rate") != 0) ? json_object_get_number(track_data, "fuel_rate") : config->defaults->fuel_rate;
	track->tyre_wear_rate			= (json_object_get_number(track_data, "tyre_wear_rate") != 0) ? json_object_get_number(track_data, "tyre_wear_rate") : config->defaults->tyre_wear_rate;
	if (json_object_dotget_boolean(track_data, "practice.enabled") == 1) {
		track->practice.name		= strdup(json_object_dotget_string(track_data, "practice.name"));
		track->practice.time		= json_object_dotget_number(track_data, "practice.time");
		track->practice.wait_time	= json_object_dotget_number(track_data, "practice.wait_time");
	}
	if (json_object_dotget_boolean(track_data, "qualify.enabled") == 1) {
		track->qualify.name			= strdup(json_object_dotget_string(track_data, "qualify.name"));
		track->qualify.time			= json_object_dotget_number(track_data, "qualify.time");
		track->qualify.wait_time	= json_object_dotget_number(track_data, "qualify.wait_time");
	}
	if (json_object_dotget_boolean(track_data, "race.enabled") == 1) {
		track->race.name			= strdup(json_object_dotget_string(track_data, "race.name"));
		track->race.time			= json_object_dotget_number(track_data, "race.time");
		track->race.wait_time		= json_object_dotget_number(track_data, "race.wait_time");
	}
	track->dynamic_track[0] 		= (json_object_dotget_number(track_data, "dynamic_track.session_start") != 0) ? json_object_dotget_number(track_data, "dynamic_track.session_start") : config->defaults->dynamic_track[0];
	track->dynamic_track[1] 		= (json_object_dotget_number(track_data, "dynamic_track.randomness") != 0) ? json_object_dotget_number(track_data, "dynamic_track.randomness") : config->defaults->dynamic_track[1];
	track->dynamic_track[2] 		= (json_object_dotget_number(track_data, "dynamic_track.lap_gain") != 0) ? json_object_dotget_number(track_data, "dynamic_track.lap_gain") : config->defaults->dynamic_track[2];
	track->dynamic_track[3] 		= (json_object_dotget_number(track_data, "dynamic_track.session_transfer") != 0) ? json_object_dotget_number(track_data, "dynamic_track.session_transfer") : config->defaults->dynamic_track[3];
	attach_to_list(track, track_list);

	return 1;
}
