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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>

#include <string.h>
#include <stdio.h>

#include "util/admdir.h"
#include "util/slist.h"
#include "util/ealloc.h"
#include "arg.h"

static void
usage() {
	fprintf(stderr, "usage: %s\n", argv0);
	exit(1);
}

int
main(int argc, char **argv) {
	struct slist_t *list = NULL;
	int err = 0;
	size_t i;

	ARGBEGIN {
	default:
		usage();
	} ARGEND;

	if (argc)
		usage();

	list = ecalloc(1, sizeof(struct slist_t));
	err |= read_adm_dir("/var/log/packages", NULL, list, 0, NULL);
	sort_slist(list);

	for (i = 0; i < list->i; ++i) {
		char *a = list->items[i].str;
		char *A = list->items[i].aux;
		size_t j;
		bool first = true;

		for (j = i + 1; j < list->i; ++j) {
			char *b = list->items[j].str;
			char *B = list->items[j].aux;

			if (strcmp(a, b))
				break;
			if (first) {
				printf("%s:%s\n", a, A);
			}
			first = false;
			printf("%s:%s\n", b, B);
			i = j;
		}
	}

	cleanup_slist(list);
	free(list);

	return err;
}
