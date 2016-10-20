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
#include "util/estrtoul.h"

#include "arg.h"

static void
print_item(const struct slist_item_t *item) {
	const char prefix[10] = "bkMGTPEZY";
	off_t val = item->siz;
	double x;
	char *bn;
	int i;

	if (item->siz == (size_t)-1)
		return;

	for (i = 0, x = val; x > 1000.0; x /= 1000.0, i++) ;

	if (i > 9)
		return;

	if (x < 10.0)
		printf("%.2f", x);
	else if (x < 100.0)
		printf("%.1f", x);
	else if (x < 1000.0)
		printf("%.0f", x);
	
	bn = strrchr(item->str, '/');
	bn = bn ? bn + 1 : item->str;

	printf("%c\t%s\n", prefix[i], bn);
}

static void
set_size(struct slist_item_t *item) {
	off_t size = 0;
	struct slist_t c;
	size_t k, inodes_n = 0;
	ino_t *inodes;

	memset(&c, 0, sizeof(c));

	if (read_adm_pkg(item->str, &c))
		return;

	inodes = ecalloc(c.i, sizeof(ino_t));

	for (k = 0; k < c.i; ++k) {
		struct stat fs;
		if (lstat(c.items[k].str, &fs) == 0) {
			bool have = false;
			size_t l;
			for (l = 0; l < inodes_n; ++l) {
				if (fs.st_ino == inodes[l]) {
					have = true;
					break;
				}
			}
			if (!have) {
				size += fs.st_size;
				inodes[inodes_n++] = fs.st_ino;
			}
		}
	}

	item->siz = size;

	free(inodes);
	cleanup_slist(&c);

}

static void
usage() {
	fprintf(stderr, "usage: %s [-S|-L#|-l|-t] [pkg ...]\n", argv0);
	exit(1);
}

int
main(int argc, char **argv) {
	struct slist_t *list = NULL;
	size_t start;
	size_t i;
	int err = 0;
	size_t limit = 0;
	enum { SORT_SIZE, SORT_TIME, SORT_NONE } sort = SORT_SIZE;

	ARGBEGIN {
	case 'l':
		sort = SORT_TIME;
		limit = 20;
		break;
	case 't':
		sort = SORT_TIME;
		break;
	case 'S':
		sort = SORT_NONE;
		break;
	case 'L':
		limit = eatoul(EARGF(usage()));
		break;
	default:
		usage();
	} ARGEND;

	list = ecalloc(1, sizeof(struct slist_t));
	err |= read_adm_dir("/var/log/packages", list, NULL, argc, argv);

	if (limit && limit < list->i) {
		start = list->i - limit;
	} else {
		start = 0;
	}

	switch (sort) {
	case SORT_TIME:
		for (i = 0; i < list->i; ++i) {
			struct stat fs;
			if (lstat(list->items[i].str, &fs) < 0) {
				/* CANTHAPPEN, unless race... */
				continue;
			}
			list->items[i].siz = fs.st_mtime;
		}
		sort_slist_siz(list);
		for (i = start; i < list->i; ++i) {
			set_size(&list->items[i]);
			print_item(&list->items[i]);
		}
		break;
	case SORT_SIZE:
		for (i = 0; i < list->i; ++i) {
			set_size(&list->items[i]);
		}
		sort_slist_siz(list);
		for (i = start; i < list->i; ++i) {
			print_item(&list->items[i]);
		}
		break;
	case SORT_NONE:
		for (i = start; i < list->i; ++i) {
			set_size(&list->items[i]);
			print_item(&list->items[i]);
		}
		break;
	}

	cleanup_slist(list);
	free(list);

	return err;
}
