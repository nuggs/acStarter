#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

char *read_file(const char * filename) {
	FILE *fp = fopen(filename, "r");
	size_t file_size;
	char *file_contents;

	if (!fp)
		return NULL;

	fseek(fp, 0L, SEEK_END);
	file_size = ftell(fp);
	rewind(fp);
	file_contents = (char*)malloc(sizeof(char) * (file_size + 1));

	if (!file_contents) {
		fclose(fp);
		return NULL;
	}

	if (fread(file_contents, file_size, 1, fp) < 1) {
		if (ferror(fp)) {
			fclose(fp);
			free(file_contents);
			return NULL;
		}
	}

	fclose(fp);
	file_contents[file_size] = '\0';
	return file_contents;
}

pid_t proc_find(const char* name) 
{
    DIR* dir;
    struct dirent* ent;
    char* endptr;
    char buf[512];

    if (!(dir = opendir("/proc"))) {
        perror("can't open /proc");
        return -1;
    }

    while((ent = readdir(dir)) != NULL) {
        /* if endptr is not a null character, the directory is not
         * entirely numeric, so ignore it */
        long lpid = strtol(ent->d_name, &endptr, 10);
        if (*endptr != '\0') {
            continue;
        }

        /* try to open the cmdline file */
        snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
        FILE* fp = fopen(buf, "r");

        if (fp) {
            if (fgets(buf, sizeof(buf), fp) != NULL) {
                /* check the first token in the file, the program name */
                char* first = strtok(buf, " ");
                if (!strcmp(first, name)) {
                    fclose(fp);
                    closedir(dir);
                    return (pid_t)lpid;
                }
            }
            fclose(fp);
        }
    }

    closedir(dir);
    return -1;
}