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

void
usage() {
	fprintf(stderr, "usage: %s <package [...]>\n", argv0);
	exit(1);
}

int
main(int argc, char *argv[]) {
	int i, ret;
	char **files;

	ARGBEGIN {
	default:
		usage();
	} ARGEND;

	if(!argc)
		usage();

	files = dst_findpkg(argc, argv);

	ret = 0;

	for(i = 0; i < argc; ++i) {
		if (files[i] == NULL) {
			fprintf(stderr, "%s:missing\n", argv[i]);
			ret = 1;
		} else {
			fprintf(stdout, "%s:%s\n", argv[i], files[i]);
			free(files[i]);
		}
	}
	free(files);

	return ret;
}
