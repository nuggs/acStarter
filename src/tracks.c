/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#include <stdio.h>
#include <stdlib.h>

#include "tracks.h"
#include "parson.h"

/* local function delclarations */
int parse_tracks(JSON_Object *track);

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
		parse_tracks(tracks_object);
	}

	return 0;
}

int parse_tracks(JSON_Object *track) {
	if (json_object_get_string(track, "track") != NULL)
		fprintf(stdout, "Loading track: %s (%g pits)\n", json_object_get_string(track, "track"), json_object_get_number(track, "max_clients"));
	return -1;
}