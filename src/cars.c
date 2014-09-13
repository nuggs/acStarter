/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "cars.h"
#include "parson.h"

LIST *skin_list;

CAR_SKINS *alloc_skin(void) {
	CAR_SKINS *skin;
	int i = 0;

	skin = malloc(sizeof(*skin));
	skin->car = NULL;
	for (i = 0; i < 15; i++)
		skin->skin[i] = NULL;
	return skin;
}

void free_skin(CAR_SKINS *skin) {
	int i = 0;

	detach_from_list(skin, skin_list);
	free(skin->car);
	for (i = 0; i < MAX_SKINS; i++) {
		if (skin->skin[i] != NULL)
			free(skin->skin[i]);
	}
}

int read_car_skins(const char *filename) {
	JSON_Value *skin_root;
	JSON_Array *skin_array, *skins_array;
	JSON_Object *skin_object;
	CAR_SKINS *skins;
	int i, x;

	skin_root = json_parse_file_with_comments(filename);
	if (json_value_get_type(skin_root) != JSONArray) {
		fprintf(stdout, "JSON is not an array\n");
		return -1;
	}

	fprintf(stdout, "Reading car skins: %s\n", filename);
	skin_array = json_value_get_array(skin_root);
	for (i=0; i < json_array_get_count(skin_array); i++) {
		skin_object = json_array_get_object(skin_array, i);
		skins = alloc_skin();
		skins->car = (json_object_get_string(skin_object, "car") != NULL) ? strdup(json_object_get_string(skin_object, "car")) : NULL;
		skins_array = json_object_get_array(skin_object, "skins");
		if (skins_array != NULL) {
			for (x = 0; x < json_array_get_count(skins_array); x++) {
				skins->skin[x] = (json_array_get_string(skins_array, x) != NULL) ? strdup(json_array_get_string(skins_array, x)) : NULL;
				printf("Skin %s [%s] loaded.\n", skins->car, skins->skin[x]);
			}
		}
		attach_to_list(skins, skin_list);
	}
	json_value_free(skin_root);
	return 0;
}

const char *random_skin(const char *car) {
	CAR_SKINS *skin = NULL;
	ITERATOR iterator;
	bool found = false;
	int i = 0, max_skin = 0;

	attach_iterator(&iterator, skin_list);
	while ((skin = (CAR_SKINS *) next_in_list(&iterator)) != NULL) {
		if (strcmp(car, skin->car) == 0) {
			for (i = 0; i < MAX_SKINS; i++) {
				if (skin->skin[i] != NULL) {
					max_skin = i;
				}
			}
			found = true;
			break;
		}
	}
	detach_iterator(&iterator);

	if (found)
		return skin->skin[random_range(0, max_skin)];

	return NULL;
}