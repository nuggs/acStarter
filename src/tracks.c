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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "core.h"
#include "config.h"
#include "tracks.h"
#include "cars.h"
#include "parson.h"

LIST *track_list = NULL;
LIST *entry_list = NULL;
struct track_data *current_track = NULL;

/* local function delclarations */
int parse_track(JSON_Object *track, int number);
int write_entry_list(void);

struct track_data *alloc_track(void) {
    struct track_data *track;
    int i;

    track = malloc(sizeof(*track));
    track->events = alloc_list();
    track->skins = NULL;
    track->entry_list = NULL;
    track->name = NULL;
    track->cars = NULL;
    track->track = NULL;
    track->sun_angle = 0;
    track->max_clients = 0;
    track->race_over_time = 0;
    track->port = 0;
    track->http_port = 0;
    track->register_to_lobby = 0;
    track->pickup_mode_enabled = 0;
    track->sleep_time = 0;
    track->voting_quorum = 0;
    track->vote_duration = 0;
    track->blacklist_mode = 0;
    track->client_send_interval_hz = 0;
    track->use_flow_control = 0;
    track->loop_mode = 0;
    track->password = NULL;
    track->allowed_tyres_out = 0;
    track->damage_multiplier = 0;
    track->fuel_rate = 0;
    track->tyre_wear_rate = 0;
    track->practice.enabled = false;
    track->practice.name = NULL;
    track->practice.time = 0;
    track->practice.wait_time = 0;
    track->qualify.enabled = false;
    track->qualify.name = NULL;
    track->qualify.time = 0;
    track->qualify.wait_time = 0;
    track->race.name = false;
    track->race.name = NULL;
    track->race.time = 0;
    track->race.wait_time = 0;
    for (i=0;i<=3;i++)
        track->dynamic_track[i] = 0;
    track->track_number = 0;
    return track;
}

void free_track(struct track_data *track) {
    struct event_data *events;
    ITERATOR iterator;

    detach_from_list(track, track_list);

    attach_iterator(&iterator, track->events);
    while ((events = (struct event_data *) next_in_list(&iterator)) != NULL)
        dequeue_event(events);
    detach_iterator(&iterator);
    free_list(track->events);

    free(track->skins);
    free(track->entry_list);
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

    if ((tracks_root = json_parse_file_with_comments(filename)) == NULL) {
        ac_log(ERROR, "read_tracklist: Failed to read track file\n");
        return -1;
    }

    if (json_value_get_type(tracks_root) != JSONArray) {
        ac_log(ERROR, "JSON is not an array\n");
        return -1;
    }

    tracks_array = json_value_get_array(tracks_root);
    for (i=0;i<json_array_get_count(tracks_array);i++) {
        tracks_object = json_array_get_object(tracks_array, i);
        parse_track(tracks_object, i);
    }

    json_value_free(tracks_root);
    return 0;
}

int parse_track(JSON_Object *track_data, int number) {
    struct track_data *track;

    if (json_object_get_string(track_data, "track") == NULL) {
        if (config->defaults != NULL) {
            ac_log(ERROR, "Defaults already populated, config has missing or null \"track\" key\n");
            return -1;
        }
        ac_log(SYSLOG, "Loading track defaults\n");
        config->defaults = alloc_track();
        config->defaults->skins = NULL;
        config->defaults->entry_list = NULL;
        config->defaults->name = strdup(json_object_get_string(track_data, "name"));
        config->defaults->cars = strdup(json_object_get_string(track_data, "cars"));
        config->defaults->track = NULL;
        config->defaults->sun_angle = floor(json_object_get_number(track_data, "sun_angle"));
        config->defaults->max_clients = floor(json_object_get_number(track_data, "max_clients"));
        config->defaults->race_over_time = floor(json_object_get_number(track_data, "race_over_time"));
        config->defaults->port = floor(json_object_get_number(track_data, "port"));
        config->defaults->http_port = floor(json_object_get_number(track_data, "http_port"));
        config->defaults->register_to_lobby = floor(json_object_get_number(track_data, "register_to_lobby"));
        config->defaults->pickup_mode_enabled = floor(json_object_get_number(track_data, "pickup_mode_enabled"));
        config->defaults->sleep_time = floor(json_object_get_number(track_data, "sleep_time"));
        config->defaults->voting_quorum = floor(json_object_get_number(track_data, "voting_quorum"));
        config->defaults->vote_duration = floor(json_object_get_number(track_data, "vote_duration"));
        config->defaults->blacklist_mode = floor(json_object_get_number(track_data, "blacklist_mode"));
        config->defaults->client_send_interval_hz = floor(json_object_get_number(track_data, "client_send_interval_hz"));
        config->defaults->use_flow_control = floor(json_object_get_number(track_data, "use_flow_control"));
        config->defaults->loop_mode = floor(json_object_get_number(track_data, "loop_mode"));
        if (json_object_get_string(track_data, "password") != NULL)
            config->defaults->password = strdup(json_object_get_string(track_data, "password"));
        config->defaults->allowed_tyres_out = floor(json_object_get_number(track_data, "allowed_tyres_out"));
        config->defaults->damage_multiplier = floor(json_object_get_number(track_data, "damage_multiplier"));
        config->defaults->fuel_rate = floor(json_object_get_number(track_data, "fuel_rate"));
        config->defaults->tyre_wear_rate = floor(json_object_get_number(track_data, "tyre_wear_rate"));
        if (json_object_dotget_boolean(track_data, "practice.enabled") == true) {
            config->defaults->practice.enabled = true;
            config->defaults->practice.name = strdup(json_object_dotget_string(track_data, "practice.name"));
            config->defaults->practice.time = floor(json_object_dotget_number(track_data, "practice.time"));
            config->defaults->practice.wait_time = floor(json_object_dotget_number(track_data, "practice.wait_time"));
        } else
            config->defaults->practice.enabled = false;
        if (json_object_dotget_boolean(track_data, "qualify.enabled") == true) {
            config->defaults->qualify.enabled = true;
            config->defaults->qualify.name = strdup(json_object_dotget_string(track_data, "qualify.name"));
            config->defaults->qualify.time = floor(json_object_dotget_number(track_data, "qualify.time"));
            config->defaults->qualify.wait_time = floor(json_object_dotget_number(track_data, "qualify.wait_time"));
        } else
            config->defaults->qualify.enabled = false;
        if (json_object_dotget_boolean(track_data, "race.enabled") == true) {
            config->defaults->race.enabled = true;
            config->defaults->race.name = strdup(json_object_dotget_string(track_data, "race.name"));
            config->defaults->race.time = floor(json_object_dotget_number(track_data, "race.laps"));
            config->defaults->race.wait_time = floor(json_object_dotget_number(track_data, "race.wait_time"));
        } else
            config->defaults->race.enabled = false;
        config->defaults->dynamic_track[0] = floor(json_object_dotget_number(track_data, "dynamic_track.session_start"));
        config->defaults->dynamic_track[1] = floor(json_object_dotget_number(track_data, "dynamic_track.randomness"));
        config->defaults->dynamic_track[2] = floor(json_object_dotget_number(track_data, "dynamic_track.lap_gain"));
        config->defaults->dynamic_track[3] = floor(json_object_dotget_number(track_data, "dynamic_track.session_transfer"));
        return 1;
    }

    ac_log(SYSLOG, "Loading track: %s (%g slots)\n", json_object_get_string(track_data, "track"), json_object_get_number(track_data, "max_clients"));
    if (json_object_get_string(track_data, "track") == NULL) {
        ac_log(ERROR, "You must specify a track, there is no default track\nTrack not loaded, loading next track\n");
        return -1;
    }

    track = alloc_track();
    track->skins = (json_object_get_string(track_data, "skins") != NULL) ? strdup(json_object_get_string(track_data, "skins")) : NULL;
    track->entry_list = strdup(json_object_get_string(track_data, "entry_list"));
    track->name = (json_object_get_string(track_data, "name") != NULL) ? strdup(json_object_get_string(track_data, "name")) : strdup(config->defaults->name);
    track->cars = (json_object_get_string(track_data, "cars") != NULL) ? strdup(json_object_get_string(track_data, "cars")) : strdup(config->defaults->cars);
    track->track = strdup(json_object_get_string(track_data, "track"));
    track->sun_angle = (json_object_get_number(track_data, "sun_angle") != 0) ? floor(json_object_get_number(track_data, "sun_angle")) : config->defaults->sun_angle;
    track->max_clients = (json_object_get_number(track_data, "max_clients") != 0) ? floor(json_object_get_number(track_data, "max_clients")) : config->defaults->max_clients;
    track->race_over_time = (json_object_get_number(track_data, "race_over_time") != 0) ? floor(json_object_get_number(track_data, "race_over_time")) : config->defaults->race_over_time;
    track->port = (json_object_get_number(track_data, "port") != 0) ? floor(json_object_get_number(track_data, "port")) : config->defaults->port;
    track->http_port = (json_object_get_number(track_data, "http_port") != 0) ? floor(json_object_get_number(track_data, "http_port")) : config->defaults->http_port;
    track->register_to_lobby = (json_object_get_number(track_data, "register_to_lobby") != 0) ? floor(json_object_get_number(track_data, "register_to_lobby")) : config->defaults->register_to_lobby;
    track->pickup_mode_enabled = (json_object_get_number(track_data, "pickup_mode_enabled") != 0) ? floor(json_object_get_number(track_data, "pickup_mode_enabled")) : config->defaults->pickup_mode_enabled;
    track->sleep_time = (json_object_get_number(track_data, "sleep_time") != 0) ? floor(json_object_get_number(track_data, "sleep_time")) : config->defaults->sleep_time;
    track->voting_quorum = (json_object_get_number(track_data, "voting_quorum") != 0) ? floor(json_object_get_number(track_data, "voting_quorum")) : config->defaults->voting_quorum;
    track->vote_duration = (json_object_get_number(track_data, "vote_duration") != 0) ? floor(json_object_get_number(track_data, "vote_duration")) : config->defaults->vote_duration;
    track->blacklist_mode = (json_object_get_number(track_data, "blacklist_mode") != 0) ? floor(json_object_get_number(track_data, "blacklist_mode")) : config->defaults->blacklist_mode;
    track->client_send_interval_hz = (json_object_get_number(track_data, "client_send_interval_hz") != 0) ? floor(json_object_get_number(track_data, "client_send_interval_hz")) : config->defaults->client_send_interval_hz;
    track->use_flow_control = (json_object_get_number(track_data, "use_flow_control") != 0) ? floor(json_object_get_number(track_data, "use_flow_control")) : config->defaults->use_flow_control;
    track->loop_mode = (json_object_get_number(track_data, "loop_mode") != 0) ? floor(json_object_get_number(track_data, "loop_mode")) : config->defaults->loop_mode;
    if (json_object_get_string(track_data, "password") != NULL)
        track->password = strdup(json_object_get_string(track_data, "password"));
    track->allowed_tyres_out = (json_object_get_number(track_data, "allowed_tyres_out") != 0) ? floor(json_object_get_number(track_data, "allowed_tyres_out")) : config->defaults->allowed_tyres_out;
    track->damage_multiplier = (json_object_get_number(track_data, "damage_multiplier") != 0) ? floor(json_object_get_number(track_data, "damage_multiplier")) : config->defaults->damage_multiplier;
    track->fuel_rate = (json_object_get_number(track_data, "fuel_rate") != 0) ? floor(json_object_get_number(track_data, "fuel_rate")) : config->defaults->fuel_rate;
    track->tyre_wear_rate = (json_object_get_number(track_data, "tyre_wear_rate") != 0) ? floor(json_object_get_number(track_data, "tyre_wear_rate")) : config->defaults->tyre_wear_rate;
    if (json_object_dotget_boolean(track_data, "practice.enabled") == true) {
        track->practice.enabled = true;
        track->practice.name = strdup(json_object_dotget_string(track_data, "practice.name"));
        track->practice.time = floor(json_object_dotget_number(track_data, "practice.time"));
        track->practice.wait_time = floor(json_object_dotget_number(track_data, "practice.wait_time"));
    } else
        track->practice.enabled = false;
    if (json_object_dotget_boolean(track_data, "qualify.enabled") == true) {
        track->qualify.enabled = true;
        track->qualify.name = strdup(json_object_dotget_string(track_data, "qualify.name"));
        track->qualify.time = floor(json_object_dotget_number(track_data, "qualify.time"));
        track->qualify.wait_time = floor(json_object_dotget_number(track_data, "qualify.wait_time"));
    } else
        track->qualify.enabled = false;
    if (json_object_dotget_boolean(track_data, "race.enabled") == true) {
        track->race.enabled = true;
        track->race.name = strdup(json_object_dotget_string(track_data, "race.name"));
        track->race.time = floor(json_object_dotget_number(track_data, "race.laps"));
        track->race.wait_time = floor(json_object_dotget_number(track_data, "race.wait_time"));
    } else
        track->race.enabled = false;
    track->dynamic_track[0] = (json_object_dotget_number(track_data, "dynamic_track.session_start") != 0) ? floor(json_object_dotget_number(track_data, "dynamic_track.session_start")) : config->defaults->dynamic_track[0];
    track->dynamic_track[1] = (json_object_dotget_number(track_data, "dynamic_track.randomness") != 0) ? floor(json_object_dotget_number(track_data, "dynamic_track.randomness")) : config->defaults->dynamic_track[1];
    track->dynamic_track[2] = (json_object_dotget_number(track_data, "dynamic_track.lap_gain") != 0) ? floor(json_object_dotget_number(track_data, "dynamic_track.lap_gain")) : config->defaults->dynamic_track[2];
    track->dynamic_track[3] = (json_object_dotget_number(track_data, "dynamic_track.session_transfer") != 0) ? floor(json_object_dotget_number(track_data, "dynamic_track.session_transfer")) : config->defaults->dynamic_track[3];
    track->track_number = number;
    attach_to_list(track, track_list);
    return 1;
}

int read_entry_list(const char *filename) {
    JSON_Value *entry_root;
    JSON_Array *entry_array;
    JSON_Object *entry_object;
    struct entry_data *entry;
    int i;

    entry_root = json_parse_file_with_comments(filename);
    if (json_value_get_type(entry_root) != JSONArray) {
        ac_log(ERROR, "JSON is not an array\n");
        return -1;
    }

    ac_log(SYSLOG, "Reading entry list: %s\n", filename);
    entry_array = json_value_get_array(entry_root);
    for (i=0; i < json_array_get_count(entry_array); i++) {
        entry_object = json_array_get_object(entry_array, i);
        entry = malloc(sizeof(*entry));
        entry->drivername = (json_object_get_string(entry_object, "drivername") != NULL) ? strdup(json_object_get_string(entry_object, "drivername")) : NULL;
        entry->team = (json_object_get_string(entry_object, "team") != NULL) ? strdup(json_object_get_string(entry_object, "team")) : NULL;
        entry->model = (json_object_get_string(entry_object, "model") != NULL) ? strdup(json_object_get_string(entry_object, "model")) : NULL;
        if ((json_object_get_string(entry_object, "skin") != NULL) && strcmp(json_object_get_string(entry_object, "skin"), "random") == 0)
            entry->skin = strdup(random_skin(entry->model));
        else
            entry->skin = (json_object_get_string(entry_object, "skin") != NULL) ? strdup(json_object_get_string(entry_object, "skin")) : NULL;
        entry->guid = (json_object_get_string(entry_object, "guid") != NULL) ? strdup(json_object_get_string(entry_object, "guid")) : NULL;
        entry->spectator_mode = floor(json_object_get_number(entry_object, "spectator_mode"));
        attach_to_list(entry, entry_list);
        ac_log(SYSLOG, "Car %s(%d)[%s] loaded.\n", entry->model, i, (entry->drivername != NULL) ? entry->drivername : "No Driver");
    }
    write_entry_list();
    json_value_free(entry_root);
    return 0;
}

int write_entry_list(void) {
    ITERATOR iterator;
    struct entry_data *entry;
    FILE *entry_config;
    char buf[4096];
    int i = 0;

    if ((check_server_config("entry_list.ini")) == -1) {
        ac_log(ERROR, "write_entry_list(): check_server_config() failed\n");
        return -1;
    }

    snprintf(buf, 4096, "%scfg/entry_list.ini", config->ac_location);
    if ((entry_config = fopen(buf, "w")) == NULL) {
        ac_log(ERROR, "Unable to write entry list for: %s\n", current_track->track);
        return -1;
    }

    attach_iterator(&iterator, entry_list);
    while ((entry = (struct entry_data *) next_in_list(&iterator)) != NULL) {
        fprintf(entry_config, "[CAR_%d]\n", i++);
        fprintf(entry_config, "DRIVERNAME=%s\n", (entry->drivername == NULL) ? "" : entry->drivername);
        fprintf(entry_config, "TEAM=%s\n", (entry->team == NULL) ? "" : entry->team);
        fprintf(entry_config, "MODEL=%s\n", (entry->model == NULL) ? "" : entry->model);
        fprintf(entry_config, "SKIN=%s\n", (entry->skin == NULL) ? "" : entry->skin);
        fprintf(entry_config, "GUID=%s\n", (entry->guid == NULL) ? "" : entry->guid);
        fprintf(entry_config, "SPECTATOR_MODE=%d\n\n", entry->spectator_mode);
    }
    detach_iterator(&iterator);
    fclose(entry_config);
    return 1;
}

int write_track(void) {
    FILE *server_config;
    char buf[4096];

    if ((check_server_config("server_cfg.ini")) == -1) {
        ac_log(ERROR, "write_track(): check_server_config() failed\n");
        return -1;
    }

    snprintf(buf, 4096, "%scfg/server_cfg.ini", config->ac_location);
    if ((server_config = fopen(buf, "w")) == NULL) {
        ac_log(ERROR, "Unable to write server config for track: %s\n", current_track->track);
        return -1;
    }

    fprintf(server_config, "[SERVER]\n");
    fprintf(server_config, "NAME=%s\n", current_track->name);
    fprintf(server_config, "CARS=%s\n", current_track->cars);
    fprintf(server_config, "TRACK=%s\n", current_track->track);
    fprintf(server_config, "SUN_ANGLE=%d\n", current_track->sun_angle);
    fprintf(server_config, "MAX_CLIENTS=%d\n", current_track->max_clients);
    fprintf(server_config, "RACE_OVER_TIME=%d\n", current_track->race_over_time);
    fprintf(server_config, "PORT=%d\n", current_track->port);
    fprintf(server_config, "HTTP_PORT=%d\n", current_track->http_port);
    fprintf(server_config, "REGISTER_TO_LOBBY=%d\n", current_track->register_to_lobby);
    fprintf(server_config, "PICKUP_MODE_ENABLED=%d\n", current_track->pickup_mode_enabled);
    fprintf(server_config, "SLEEP_TIME=%d\n", current_track->sleep_time);
    fprintf(server_config, "VOTING_QUORUM=%d\n", current_track->voting_quorum);
    fprintf(server_config, "VOTE_DURATION=%d\n", current_track->vote_duration);
    fprintf(server_config, "BLACKLIST_MODE=%d\n", current_track->blacklist_mode);
    fprintf(server_config, "CLIENT_SEND_INTERVAL_HZ=%d\n", current_track->client_send_interval_hz);
    fprintf(server_config, "USE_FLOW_CONTROL=%d\n", current_track->use_flow_control);
    fprintf(server_config, "LOOP_MODE=%d\n", current_track->loop_mode);
    fprintf(server_config, "PASSWORD=%s\n", (current_track->password != NULL) ? current_track->password : "");
    fprintf(server_config, "ALLOWED_TYRES_OUT=%d\n", current_track->allowed_tyres_out);
    fprintf(server_config, "DAMAGE_MULTIPLIER=%d\n", current_track->damage_multiplier);
    fprintf(server_config, "FUEL_RATE=%d\n", current_track->fuel_rate);
    fprintf(server_config, "TYRE_WEAR_RATE=%d\n\n", current_track->tyre_wear_rate);
    if (current_track->practice.enabled == true) {
        fprintf(server_config, "[PRACTICE]\n");
        fprintf(server_config, "NAME=%s\n", current_track->practice.name);
        fprintf(server_config, "TIME=%d\n", current_track->practice.time);
        fprintf(server_config, "WAIT_TIME=%d\n\n", current_track->practice.wait_time);
    }
    if (current_track->qualify.enabled == true) {
        fprintf(server_config, "[QUALIFY]\n");
        fprintf(server_config, "NAME=%s\n", current_track->qualify.name);
        fprintf(server_config, "TIME=%d\n", current_track->qualify.time);
        fprintf(server_config, "WAIT_TIME=%d\n\n", current_track->qualify.wait_time);
    }
    if (current_track->race.enabled == true) {
        fprintf(server_config, "[RACE]\n");
        fprintf(server_config, "NAME=%s\n", current_track->race.name);
        fprintf(server_config, "LAPS=%d\n", current_track->race.time);
        fprintf(server_config, "WAIT_TIME=%d\n\n", current_track->race.wait_time);
    }
    fprintf(server_config, "[DYNAMIC_TRACK]\n");
    fprintf(server_config, "SESSION_START=%d\n", current_track->dynamic_track[0]);
    fprintf(server_config, "RANDOMNESS=%d\n", current_track->dynamic_track[1]);
    fprintf(server_config, "LAP_GAIN=%d\n", current_track->dynamic_track[2]);
    fprintf(server_config, "SESSION_TRANSFER=%d\n", current_track->dynamic_track[3]);
    fclose(server_config);

    if (current_track->skins != NULL) {
        if ((read_car_skins(current_track->skins)) == -1) {
            ac_log(ERROR, "failed to read car skins\n");
        }
    }

    if ((read_entry_list(current_track->entry_list)) == -1) {
        ac_log(ERROR, "failed to read entry_list %s\n", current_track->entry_list);
    }
    return 1;
}

/* this is a hack of a function */
void next_track(void) {
    ITERATOR iterator;
    struct track_data *track;
    struct event_data *events;
    bool track_set = false; /* lol */
    int next_track = current_track->track_number-1;

    /* strip events from the current track */
    attach_iterator(&iterator, current_track->events);
    while ((events = (struct event_data *) next_in_list(&iterator)) != NULL)
        dequeue_event(events);
    detach_iterator(&iterator);
    free_list(current_track->events);

    /* Restart from the beginning of the list since we're on the last track */
    if (current_track == track_list->last_cell->content) {
        current_track = alloc_track();
        current_track = track_list->first_cell->content;
        track_set = true;
    }

    if (track_set == false) {
        /* Find the next track since we weren't at the end of the list */
        attach_iterator(&iterator, track_list);
        while ((track = (struct track_data *) next_in_list(&iterator)) != NULL) {
            if (track->track_number == next_track) {
                current_track = alloc_track();
                current_track = track;
            }
        }
        detach_iterator(&iterator);
    }
    free_list(entry_list);
    entry_list = alloc_list();
    write_track();
    init_events_track(current_track);
    ac_log(SYSLOG, "New track: %s\n", current_track->track);
}

