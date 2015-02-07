/*
 * Copyright © 2012-2014 Lars Lindqvist
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

#define _XOPEN_SOURCE 500
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

#include "util.h"

#define ADM_DIR "/var/log/packages"

struct Pkg {
	char *file;
	char *name;
	struct Pkg *next;
};
struct Pkg *head = NULL;

static void
usage() {
	fprintf(stderr, "usage: %s <package name [...]>\n", argv0);
	exit(1);
}

static int
populate() {
	struct dirent *dc;
	DIR *dp;
	struct Pkg *pkg = NULL;

	if(!(dp = opendir(ADM_DIR))) {
		fprintf(stderr, "%s: FATAL: opendir %s: ", argv0, ADM_DIR);
		perror(NULL);
	}

	while((dc = readdir(dp))) {
		pkg = malloc(sizeof(struct Pkg));
		pkg->file = strdup(dc->d_name);
		pkg->name = dst_packname(pkg->file, SPKG_NAME);
		pkg->next = head;
		head = pkg;
	}

	closedir(dp);

	return 0;
}

struct Pkg *
find_pkg(char *name) {
	size_t len = strlen(name) + 1;
	struct Pkg *pkg;

	for(pkg = head; pkg; pkg = pkg->next) {
		if(!strncmp(name, pkg->file, len)
		|| !strncmp(name, pkg->name, len)) {
			return pkg;
		}
	}

	return 0;
}

void
cleanup() {
	struct Pkg *pkg = NULL;

	while(head) {
		pkg = head->next;
		free(head->name);
		free(head->file);
		free(head);

		head = pkg;
	}
}

int
main(int argc, char *argv[]) {
	int i, ret;
	char *name;
	struct Pkg *pkg;

	ret = 0;

	ARGBEGIN {
	default:
		usage();
	} ARGEND;

	if(!argc)
		usage();

	populate();

	for(i = 0; i < argc; ++i) {
		name = dst_packname(argv[i], SPKG_BASE);
		if(!name) {
			fprintf(stderr, "%s: ERROR: unable to parse %s\n", argv0, argv[i]);
			continue;
		}
		pkg = find_pkg(name);

		if(pkg) {
			fprintf(stdout, "%s:%s/%s\n", argv[i], ADM_DIR, pkg->file);
		} else {
			fprintf(stderr, "%s:missing\n", argv[i]);
			ret = 1;
		}
		free(name);
	}

	cleanup();

	return ret;
}
