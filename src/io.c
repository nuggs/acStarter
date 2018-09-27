/*
 * acStarter - A simple server manager for Assetto Corsa.
 * Copyright (c) 2014 Turncoat Tony
 *
 * See the LICENSE file for license information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <time.h>

#include "core.h"
#include "config.h"
#include "io.h"

struct log_data log_type[] = {
	{"SYSLOG"},
	{"NETLOG"},
	{"ERROR"},
	{"DEBUG"}
};

void ac_log(int type, const char *txt, ...) {
	char tmp[256], buf[256], log_file[256];
	va_list ap;
	FILE *fp;

	va_start(ap, txt);
	vsnprintf (buf, 256, txt, ap);
	va_end(ap);

	strftime(tmp, 256, "%D %R: ", localtime(&current_time));
	switch(type) {
		case SYSLOG:
			snprintf(log_file, 256, "%ssyslog", config->logs);
			if ((fp = fopen(log_file, "a")) == NULL) {
				fprintf(stdout, "ac_log: Failed to open %s\n", log_file);
				return;
			}
		break;
		case NETLOG:
			snprintf(log_file, 256, "%snetlog", config->logs);
			if ((fp = fopen(log_file, "a")) == NULL) {
				fprintf(stdout, "ac_log: Failed to open %s\n", log_file);
				return;
			}
		break;
		case ERROR:
			snprintf(log_file, 256, "%serror", config->logs);
			if ((fp = fopen(log_file, "a")) == NULL) {
				fprintf(stdout, "ac_log: Failed to open %s\n", log_file);
				return;
			}
		break;
		case DEBUG:
			snprintf(log_file, 256, "%sdebug", config->logs);
			if ((fp = fopen(log_file, "a")) == NULL) {
				fprintf(stdout, "ac_log: Failed to open %s\n", log_file);
				return;
			}
		break;
		default:
			printf("ERROR: Incorrect log type\n");
		break;
	}
	if (quiet_mode == false)
		fprintf(stdout, "[%s] %s%s", log_type[type].name, tmp, buf);

	fprintf(fp, "[%s] %s%s", log_type[type].name, tmp, buf);
	fclose(fp);
}

pid_t proc_find(const char *name)  {
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
			if ((fscanf(fp, "%ld (%[^)]) %c", &pid, pname, &state)) != 3) {
				ac_log(ERROR, "fscanf failed \n");
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
