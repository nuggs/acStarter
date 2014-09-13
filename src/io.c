/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

pid_t proc_find(const char* name) 
{
	DIR* dir;
	struct dirent* ent;
	char buf[512];
	long  pid;
	char pname[100] = {0,};
	char state;
	FILE *fp=NULL; 

	if (!(dir = opendir("/proc"))) {
		perror("can't open /proc");
		return -1;
	}

	while((ent = readdir(dir)) != NULL) {
		long lpid = atol(ent->d_name);
		if(lpid < 0)
			continue;
		snprintf(buf, sizeof(buf), "/proc/%ld/stat", lpid);
		fp = fopen(buf, "r");

		if (fp) {
			if ( (fscanf(fp, "%ld (%[^)]) %c", &pid, pname, &state)) != 3 ){
				printf("fscanf failed \n");
				fclose(fp);
				closedir(dir);
				return -1; 
			}
			if (!strcmp(pname, name)) {
				fclose(fp);
				closedir(dir);
				return (pid_t)lpid;
			}
			fclose(fp);
		}
	}
	closedir(dir);
	return -1;
}