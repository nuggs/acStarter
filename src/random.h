/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#ifndef RANDOM_H
#define RANDOM_H

int random_range(int a, int b);
int random_percent(void);
int roll_dice(int amount, int size);
void init_dice(void);

#endif
