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
const char *VERSION = "0.3.6";
const int REMOVE_CFG_SERVER = 0;
const int REMOVE_CFG_ENTRY = 1;
const int REMOVE_CFG_BOTH = 2;

int remove_file(const char *filename) {
	if (remove(filename) != 0) {
		fprintf(stdout, "remove_file: Failed to remove %s\n", filename);
		return -1;
	}
	return 1;
}
/*
 * checks ac_location/cfg for server_cfg.ini and entry_list.ini
 * if it finds them it will rename them to their name followed by
 * .bak and then remove the original files.
 *
 * filename: server_cfg.ini or entry_list.ini
 */
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
		return 1;
	} else
		printf("Failed to rename %s", buf);

	return -1;
}

void remove_server_config(int file) {
	char buf[124];

	if (file == REMOVE_CFG_SERVER || file == REMOVE_CFG_BOTH) {
		snprintf(buf, 124, "%scfg/server_cfg.ini", config->ac_location);
		if (remove_file(buf) == -1) {
			fprintf(stdout, "Failed to remove: %s\n", buf);
		}
	}

	if (file == REMOVE_CFG_ENTRY || file == REMOVE_CFG_BOTH) {
		memset(buf, 0, sizeof(buf));
		snprintf(buf, 124, "%scfg/entry_list.ini", config->ac_location);
		if (remove_file(buf) == -1) {
			fprintf(stdout, "Failed to remove: %s\n", buf);
		}
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
