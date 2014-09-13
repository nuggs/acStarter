/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

typedef struct car_skins {
	char *car;
	char *skin[15];
} CAR_SKINS;

int read_car_skins(const char *filename);
const char *random_skin(const char *car);