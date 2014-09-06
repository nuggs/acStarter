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
const char *VERSION = "0.3.4";

int check_server_config(const char *filename) {
	FILE *server_config;
	char buf[120], buf2[120];

	if (filename == NULL) {
		fprintf(stdout, "check_server_config: filename is NULL\n");
		return -1;
	}

	snprintf(buf, sizeof(buf), "%scfg/%s", config->ac_location, filename);
	if ((server_config = fopen(buf, "r")) == NULL)
		return 1;

	fclose(server_config);

	fprintf(stdout, "Found %s at %s\nRenaming and backing up...\n", buf, filename);
	snprintf(buf2, sizeof(buf2), "%s.bak", buf);
	if ((rename(buf, buf2)) == 0) {
		printf("Renamed: %s\nTo: %s\n", buf, buf2);
		remove(buf);
		return 1;
	} else
		printf("Failed to rename %s", buf);

	return -1;
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
	config->base_dir	= strdup(json_object_get_string(config_json, "base_dir"));
	config->ac_exe 		= strdup(json_object_get_string(config_json, "ac_exe"));
	config->ac_location = strdup(json_object_get_string(config_json, "ac_location"));
	config->tracklist 	= strdup(json_object_get_string(config_json, "tracklist"));
	config->defaults	= NULL;

	json_value_free(root);
	return 0;
}

void free_config(void) {
	free(config->base_dir);
	free(config->ac_exe);
	free(config->ac_location);
	free(config->tracklist);
	free_track(config->defaults);
	free(config);
}
