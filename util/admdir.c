#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>

#include "slist.h"
#include "ealloc.h"

int
read_adm_pkg(const char *path, struct slist_t *list) {
	FILE *fp = fopen(path, "r");
	int err = 0;
	size_t len = 0;
	ssize_t bytes;
	char *line = NULL;
	bool start = false;
	const char *bn;
	
	bn = strrchr(path, '/');
	bn = bn ? bn + 1 : path;

	if (!fp) {
		fprintf(stderr, "ERROR: fopen %s: %s\n", path, strerror(errno));
		return -1;
	}
	
	while ((bytes = getline(&line, &len, fp)) > 0) {
		char *c;
		char *real;

		if (!start) {
			if (!strcmp(line, "FILE LIST:\n"))
				start = true;
			continue;
		}

		if (!strcmp(line, "./\n"))
			continue;
		if (!strncmp(line, "install/", 8))
			continue;
		if (bytes < 2)
			continue;
		if (line[bytes - 2] == '/')
			continue;

		line[bytes - 1] = 0;

		if ((c = strstr(line, "/incoming/"))) {
			char *src = c + strlen("/incoming");
			memmove(c, src, strlen(src) + 1);
		}
		if ((c = strstr(line, ".new")) && strlen(c) == 4) {
			*c = '\0';
		}
	
		real = (char*)emalloc(1 + strlen(line) + 1);
		sprintf(real, "/%s", line);

		add_to_slist(list, real, strdup(bn), 1000);
	}
	free(line);

	if (ferror(fp)) {
		fprintf(stderr, "ERROR: getline %s: %s\n", path, strerror(errno));
		err |= 1;
	}

	fclose(fp);

	return err;
}

static const char *
match_pkgname(const char *pkg, int pkgc, char **pkgv) {
	int i;

	for (i = 0; i < pkgc; ++i) {
		const char *cmp = pkg;
		const char *bn = strrchr(pkgv[i], '/');
		bn = bn ? bn + 1 : pkgv[i];

		while (*bn == *cmp) {
			if (*bn == '\0')
				return pkgv[i];
			bn++;
			cmp++;
		}

		if (!strcmp(bn, ".tgz") || !strcmp(bn, ".txz")) {
			return pkgv[i];
		}

		if (*cmp == '-' && !*bn) {
			int seps = 1;
			while (*cmp++)
				if (*cmp == '-')
					++seps;
			if (seps == 3)
				return pkgv[i];
		}
	}
	return NULL;
}

/*
 * Fills `pkg_list` with "[match\0]<dir>/<pkg>"
 */

int
read_adm_dir(const char *dir, struct slist_t *pkg_list, struct slist_t *content_list, int pkgc, char **pkgv) {
	int err = 0;
	struct dirent *dc;
	DIR *dp;

	if (!(dp = opendir(dir))) {
		fprintf(stderr, "ERROR: opendir %s: %s\n", dir, strerror(errno));
		return -1;
	}

	while ((dc = readdir(dp))) {
		const char *match = NULL;
		char *path;

		if (dc->d_name[0] == '.')
			continue;

		if (pkgc && !(match = match_pkgname(dc->d_name, pkgc, pkgv))) {
			continue;
		}

		path = (char*)emalloc(strlen(dir) + strlen(dc->d_name) + 2);
		sprintf(path, "%s/%s", dir, dc->d_name);

		if (pkg_list) {
			char *aux = NULL;
			if (match)
				aux = strdup(match);
			add_to_slist(pkg_list, strdup(path), aux, 100);
		}

		if (content_list)
			err |= read_adm_pkg(path, content_list);

		free(path);
	}

	if (closedir(dp) < 0) {
		fprintf(stderr, "ERROR: closedir %s: %s\n", dir, strerror(errno));
		return -1;
	}

	return err;
}
