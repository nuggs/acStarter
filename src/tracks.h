/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef TRACKS_H
#define TRACKS_H

extern struct track_data *current_track;

struct mode_data {
    bool enabled;
    char *name;
    int time;
    int wait_time;
};

struct track_data {
    LIST *events;
    char *skins;
    char *entry_list;
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
    struct mode_data practice;
    struct mode_data qualify;
    struct mode_data race;
    int dynamic_track[3];
    int track_number;
};

struct entry_data {
    char *drivername;
    char *team;
    char *model;
    char *skin;
    char *guid;
    int spectator_mode;
};

/* global file delcarations */
struct track_data *alloc_track(void);
void free_track(struct track_data *track);
int read_tracklist(const char *filename);
int write_track(void);
void next_track(void);

#endif
