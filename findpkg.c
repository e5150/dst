/* Copyright © 2012-2014 Lars Lindqvist
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include "arg.h"
#include "util.h"

#define ADM_DIR "/var/log/packages"

struct Pkg {
	char *file;
	char *name;
	char *base;
	struct Pkg *next;
};
struct Pkg *head = NULL;

static int
populate() {
	struct dirent *dc;
	DIR *dp;
	struct Pkg *pkg = NULL;

	if(!(dp = opendir(ADM_DIR))) {
		fprintf(stderr, "%s: FATAL: opendir %s: %s\n", argv0, ADM_DIR, strerror(errno));
		exit(1);
	}

	while((dc = readdir(dp))) {
		size_t len;

		pkg = malloc(sizeof(struct Pkg));

		pkg->name = dst_packname(dc->d_name, SPKG_NAME);
		pkg->base = dst_packname(dc->d_name, SPKG_BASE);

		len = strlen(ADM_DIR) + 1 + strlen(dc->d_name) + 1;
		pkg->file = malloc(len * sizeof(char));
		snprintf(pkg->file, len, "%s/%s", ADM_DIR, dc->d_name);

		pkg->next = head;
		head = pkg;
	}

	if(closedir(dp) == -1) {
		fprintf(stderr, "%s: FATAL: closedir %s: %s", argv0, ADM_DIR, strerror(errno));
		exit(1);
	}

	return 0;
}

static struct Pkg *
find_pkg(char *name) {
	size_t len = strlen(name) + 1;
	struct Pkg *pkg;

	for(pkg = head; pkg; pkg = pkg->next) {
		if(!strncmp(name, pkg->file, len)
		|| !strncmp(name, pkg->name, len)
		|| !strncmp(name, pkg->base, len)) {
			return pkg;
		}
	}

	return NULL;
}

static void
cleanup() {
	struct Pkg *pkg = NULL;

	while(head) {
		pkg = head->next;
		free(head->name);
		free(head->base);
		free(head->file);
		free(head);

		head = pkg;
	}
}

char **
dst_findpkg(size_t argc, char **argv) {
	char *name;
	struct Pkg *pkg;
	size_t i;
	char **ret;

	ret = calloc(argc, sizeof(char*));

	populate();

	for (i = 0; i < argc; ++i) {
		ret[i] = NULL;

		if ((name = dst_packname(argv[i], SPKG_BASE))) {
			if ((pkg = find_pkg(name))) {
				ret[i] = strdup(pkg->file);
			}
			free(name);
		} else {
			fprintf(stderr, "findpkg: ERROR: Unable to parse %s\n", argv[i]);
		}
	}

	cleanup();

	return ret;
}
