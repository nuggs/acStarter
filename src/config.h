/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */
#ifndef CONFIG_H
#define CONFIG_H

extern const char *VERSION;
extern const int REMOVE_CFG_SERVER;
extern const int REMOVE_CFG_ENTRY;
extern const int REMOVE_CFG_BOTH;

typedef struct {
	char *base_dir;
	char *ac_exe;
	char *ac_location;
	char *tracklist;
	struct track_data *defaults;
} config_data;

config_data *config;
int remove_file(const char *filename);
int check_server_config(const char *filename);
void remove_server_config(int file);
int read_config(const char *filename);
void free_config(void);

#endif