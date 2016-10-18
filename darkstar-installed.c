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

#include <stdlib.h>
#include <stdio.h>

#include "util/admdir.h"
#include "util/slist.h"
#include "util/ealloc.h"

#include "arg.h"

static void
usage(const char *help) {
	fprintf(stderr, "usage: %s %s\n", argv0, help);
	exit(1);
}

int
main(int argc, char **argv) {
	struct slist_t *list = NULL;
	const char help[] = "<pkg> ...";
	int err = 0;
	int i;

	ARGBEGIN {
	default:
		usage(help);
	} ARGEND;

	if (!argc)
		usage(help);

	list = ecalloc(1, sizeof(struct slist_t));
	err |= read_adm_dir("/var/log/packages", list, NULL, argc, argv);
	sort_slist(list);

	for (i = 0; i < argc; ++i) {
		struct slist_item_t *match;
		match = bsearch_slist_aux(argv[i], list);
		if (!match) {
			puts(argv[i]);
			err |= 1;
		}
	}

	cleanup_slist(list);
	free(list);

	return err;
}
