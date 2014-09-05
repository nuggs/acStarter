
#ifndef TRACKS_H
#define TRACKS_H

typedef struct track_data TRACK;

struct track_data {
	TRACK *prev;
	TRACK *next;
};

/* global file delcarations */
int read_tracklist(const char *filename);

#endif