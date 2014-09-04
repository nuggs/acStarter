/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "config.h"
#include "nxjson.h"
#include "io.h"

/* global version variable */
const char *VERSION = "0.0.1";

/*
 * Reads in the configuration file populating the configuration
 * structure with the data from the configuration file.
 */
int read_config(const char *filename) {
	char *config_file = read_file(filename); // "/home/seventen/acstarter/cfg/race.cfg"
	const nx_json *config_input;

	/* exit out of the program if we failed to read the file */
	if (config_file == NULL) {
		return -1;
	}

	/* parse the json using nx_json parser */
	config_input = nx_json_parse(config_file, 0);
	if (config_input) {
		/* set the configuration data */
		config 				= malloc(sizeof(config_data*));
		config->exe 		= nx_json_get(config_input, "exe")->text_value;
		config->location 	= nx_json_get(config_input, "location")->text_value;
		config->tracklist 	= nx_json_get(config_input, "tracklist")->text_value;

		nx_json_free(config_input);
		return 0;
	}

	if (config_input)
		nx_json_free(config_input);

	return -1;
}