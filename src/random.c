/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#include <sys/types.h>
#include <stdio.h>
#include <time.h>

#include "core.h"

static unsigned int iSeed;
double rand_seed(void);

/* returns a random number between a and b */
int random_range(int a, int b) {
	return ((int)(rand_seed() * ((double) (b) - (a) + 1))) + (a);
}

/* return a random number between 1 and 100 */
int random_percent(void) {
	return random_range(1, 100);
}

/* Roll some amount of dices of a specific size */
int roll_dice(int amount, int size) {
	int i;
	int result = 0;

	for (i = 1; i <= amount; i++) {
		result += random_range(1, size);
	}
	return result;
}

/* this function initializes the randomize functions */
void init_dice(void) {
	iSeed = (unsigned int) time(NULL);
}

/* Local function that returns a random double between 0 and 1 */
double rand_seed(void) {
	static unsigned int a = 1588635695, m = 4294967291U, q = 2, r = 1117695901;

	iSeed = a * (iSeed % q) - r * (iSeed / q);
	return ((double) iSeed / (double) m);
}
