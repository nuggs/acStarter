/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

/*
 * Adjust this as needed, it's a pain hard coded but oh well...
 * Would much rather use a constant here but well...  /sadface
 */
#define MAX_SKINS 15

typedef struct car_skins {
	char *car;
	char *skin[MAX_SKINS];
} CAR_SKINS;

int read_car_skins(const char *filename);
const char *random_skin(const char *car);
void free_skin(CAR_SKINS *skin);