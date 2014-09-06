/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "parson.h"
#include "tracks.h"
#include "config.h"
#include "io.h"

/* global version variable */
const char *VERSION = "0.0.1";

void check_server_config(void) {
	FILE *server_config;
	char buf[120];

	sprintf(buf, "%scfg/server_cfg.ini", config->location);
	if ((server_config = fopen(buf, "r"))) {
		fclose(server_config);
		fprintf(stdout, "Found server_cfg.ini at %scfg/server_cfg.ini\n", config->location);
	}
}

int read_config(const char *filename) {
	JSON_Value *root;
	JSON_Object *config_json;

	if ((root = json_parse_file_with_comments(filename)) == NULL) {
		return -1;
	}

	if ((config_json = json_value_get_object(root)) == NULL) {
		return -1;
	}

	config 				= malloc(sizeof(*config));
	config->exe 		= strdup(json_object_get_string(config_json, "exe"));
	config->location 	= strdup(json_object_get_string(config_json, "location"));
	config->tracklist 	= strdup(json_object_get_string(config_json, "tracklist"));
	config->defaults	= NULL;

	json_value_free(root);
	return 0;
}

void free_config(void) {
	free(config->exe);
	free(config->location);
	free(config->tracklist);
	free_track(config->defaults);
	free(config);
}