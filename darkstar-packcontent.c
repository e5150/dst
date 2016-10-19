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
	fprintf(stderr, "usage: %s [-v] [-e|-m] [-f file | pkg ...]\n", argv0);
	exit(1);
}

int
main(int argc, char **argv) {
	struct slist_t *list = NULL;
	const char *regex_str = NULL;
	regex_t regex;
	int err = 0;
	size_t i;
	bool fflag = false;
	bool eflag = false;
	bool mflag = false;
	bool verbose = false;

	ARGBEGIN {
	case 'f':
		fflag = true;
		regex_str = EARGF(usage());
		break;
	case 'v':
		verbose = true;
		break;
	case 'e':
		eflag = true;
		break;
	case 'm':
		mflag = true;
		break;
	default:
		usage();
	} ARGEND;

	if (eflag && mflag)
		usage();
	if (fflag) {
		verbose = true;
		if (argc)
			usage();
		if (regcomp(&regex, regex_str, REG_EXTENDED | REG_NOSUB) != 0) {
			fprintf(stderr, "%s: ERROR: Invalid regex: %s\n", argv0, regex_str);
			exit(1);
		}
	}

	list = ecalloc(1, sizeof(struct slist_t));
	err |= read_adm_dir("/var/log/packages", NULL, list, argc, argv);

	for (i = 0; i < list->i; ++i) {
		const char *file = list->items[i].str;
		const char *pkg  = list->items[i].aux;
		bool print = !eflag && !mflag;
		
		if (!print) {
			bool e = access(file, F_OK) == 0;
			print = eflag && e || mflag && !e;
		}
		if (fflag && regexec(&regex, file, 0, NULL, 0) == REG_NOMATCH)
			print = false;
		if (print) {
			if (verbose) {
				printf("%s:%s\n", file, pkg);
			} else {
				puts(file);
			}
		}
	}

	if (fflag)
		regfree(&regex);
	cleanup_slist(list);
	free(list);

	return err;
}
