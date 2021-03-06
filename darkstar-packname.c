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
#include <string.h>
#include <stdio.h>

#include "arg.h"

static void
usage(const char *help) {
	fprintf(stderr, "usage: %s %s\n", argv0, help);
	exit(1);
}

int
main(int argc, char **argv) {
	const char help[] = "[-v|-b] pkg ...";
	int err = 0;
	int i;
	bool bflag = false;
	bool vflag = false;

	ARGBEGIN {
	case 'v':
		vflag = true;
		break;
	case 'b':
		bflag = true;
		break;
	default:
		usage(help);
	} ARGEND;

	if (!argc)
		usage(help);

	for (i = 0; i < argc; ++i) {
		size_t j, sep_n;
		char *bn = strrchr(argv[i], '/');
		bn = bn ? bn + 1 : argv[i];

		if (bflag) {
			char *ext;
			if ((ext = strstr(bn, ".tgz"))
			 || (ext = strstr(bn, ".txz"))
			 || (ext = strstr(bn, ".tbz"))
			 || (ext = strstr(bn, ".tlz"))) {
				*ext = 0;
			}
		} else {
			for (sep_n = 0, j = strlen(bn); sep_n < 3 && j > 0; --j) {
				if (bn[j] == '-') {
					++sep_n;
					bn[j] = 0;
				}
			}
			if (vflag) {
				bn += j + 2;
			}
		}
		puts(bn);
	}

	return err;
}

