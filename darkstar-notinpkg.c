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

static int
mcn(const char *path, dev_t rootdev, struct slist_t *files) {
	struct stat fs;
	int err = 0;

	if (lstat(path, &fs) == -1) {
		fprintf(stderr, "%s: ERROR: lstat %s: %s\n", argv0, path, strerror(errno));
		return 1;
	}

	if (S_ISREG(fs.st_mode)) {
		add_to_slist(files, strdup(path), NULL, 1000);
	}
	else if (S_ISDIR(fs.st_mode)) {
		struct dirent *c;
		DIR *dp;

		if (fs.st_dev != rootdev) {
			return 0;
		}

		if ((dp = opendir(path)) == NULL) {
			fprintf(stderr, "%s: ERROR: opendir %s: %s\n", argv0, path, strerror(errno));
			return 1;
		}

		while ((c = readdir(dp))) {
			char fullpath[PATH_MAX];

			if (!strcmp(c->d_name, ".") || !strcmp(c->d_name, "..")) {
				continue;
			}

			snprintf(fullpath, PATH_MAX, "%s/%s%c",
				 !strcmp(path, "/") ? "" : path,
				 c->d_name, '\0');

			err |= mcn(fullpath, rootdev, files);
		}
		if (closedir(dp) == -1) {
			fprintf(stderr, "%s: ERROR: closedir %s: %s\n", argv0, path, strerror(errno));
			err |= 1;
		}
	} else {
		return 0;
	}
	
	return err;
}

static int
xmcn(const char *path, struct slist_t *list) {
	struct stat fs;
	if (stat(path, &fs) == -1) {
		fprintf(stderr, "%s: ERROR: stat %s: %s\n", argv0, path, strerror(errno));
		return -1;
	}
	return mcn("/", fs.st_dev, list);
}

static void
print_missing_from_2nd(const struct slist_t *list_a, const struct slist_t *list_b, const regex_t *ignore) {
	size_t a_i;
	size_t b_i;

	for (b_i = a_i = 0; a_i < list_a->i; ++a_i) {
		const char *str;
		bool found;

		while (a_i + 1 < list_a->i) {
			if (strcmp(list_a->items[a_i].str, list_a->items[a_i + 1].str))
				break;
			a_i++;
		}
		str = list_a->items[a_i].str;

		for (found = false; !found && b_i < list_b->i; ++b_i) {
			int cmp = strcmp(str, list_b->items[b_i].str);
			if (cmp == 0)
				found = true;
			if (cmp <= 0)
				break;
		}
		if (!found && regexec(ignore, str, 0, NULL, 0) == REG_NOMATCH) {
			puts(str);
		}
	}
}

static void
usage() {
	fprintf(stderr, "usage: %s [-e exclusion-regex]\n", argv0);
	exit(1);
}

int
main(int argc, char **argv) {
	const char *regex_str = "^/(var/|mnt/|etc/|boot/(map|boot.[0-9]*))";
	int err = 0;
	struct slist_t *sys_list;
	struct slist_t *pkg_list;
	regex_t regex;

	ARGBEGIN {
	case 'e':
		regex_str = EARGF(usage());
		break;
	default:
		usage();
	} ARGEND;

	if (argc)
		usage();

	if (regcomp(&regex, regex_str, REG_EXTENDED | REG_NOSUB)) {
		fprintf(stderr, "%s: ERROR: Invalid regex: %s\n", argv0, regex_str);
		return -1;
	}

	sys_list = ecalloc(1, sizeof(struct slist_t));
	pkg_list = ecalloc(1, sizeof(struct slist_t));

	err |= read_adm_dir("/var/log/packages", NULL, pkg_list, 0, NULL);

	if (argc) {
		int i;
		for (i = 0; i < argc; ++i) {
			err |= xmcn(argv[i], sys_list);
		}
	} else {
		err |= xmcn("/", sys_list);
	}

	sort_slist(pkg_list);
	sort_slist(sys_list);

	if (strstr(argv0, "notinpkg")) {
		print_missing_from_2nd(sys_list, pkg_list, &regex);
	} else {
		print_missing_from_2nd(pkg_list, sys_list, &regex);
	}
	regfree(&regex);
	cleanup_slist(sys_list);
	cleanup_slist(pkg_list);
	free(sys_list);
	free(pkg_list);


	return err;
}

