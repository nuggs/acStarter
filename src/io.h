/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#ifndef IO_H
#define IO_H

struct log_data {
	char *name;
	char *location;
};

/* function declarations */
pid_t proc_find(const char* name);

#endif
