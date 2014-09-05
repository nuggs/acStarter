/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */
#ifndef CONFIG_H
#define CONFIG_H

extern const char *VERSION;

typedef struct {
	char *exe;
	char *location;
	char *tracklist;
	struct track_data *defaults;
} config_data;

config_data *config;
int read_config(const char *filename);
void free_config(void);

#endif