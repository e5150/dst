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

#include <string.h>
#include <stdio.h>

#include "util.h"

void
usage() {
	fprintf(stderr, "usage: %s [-n|-v|-V|-a|-b|-t|-e|-B] <package file [...]>\n", argv0);
	exit(1);
}

#define VERBOSE(str, what) { \
	char *sect = dst_packname((str), SPKG_##what);         \
	fprintf(stdout, "%s:%s:%s\n", (str), # what, sect ? sect : ""); \
	if(sect) free(sect); \
} 

int
main(int argc, char *argv[]) {
	int i;
	int verbose = 0;
	char *str;
	enum SPKG mode = SPKG_NULL;

	ARGBEGIN {
	case 'v':
		verbose = 1;
		break;
	case 'V':
		mode = SPKG_VERSION;
		break;
	case 'a':
		mode = SPKG_ARCH;
		break;
	case 'B':
		mode = SPKG_BUILD;
		break;
	case 't':
		mode = SPKG_TAG;
		break;
	case 'e':
		mode = SPKG_EXT;
		break;
	case 'b':
		mode = SPKG_BASE;
		break;
	case 'n':
		mode = SPKG_NAME;
		break;
	default:
		usage();
	} ARGEND;

	if(!verbose && mode == SPKG_NULL) {
		mode = SPKG_NAME;
	}

	if(!argc)
		usage();
	for(i = 0; i < argc; ++i) {
		if(verbose) {
			VERBOSE(argv[i], NAME);
			VERBOSE(argv[i], VERSION);
			VERBOSE(argv[i], ARCH);
			VERBOSE(argv[i], BUILD);
			VERBOSE(argv[i], TAG);
			VERBOSE(argv[i], EXT);
			VERBOSE(argv[i], BASE);
		} else {
			str = dst_packname(argv[i], mode);

			if(str) {
				fprintf(stdout, "%s\n", str);
				free(str);
			} else {
				fprintf(stdout, "\n");
			}
		}
	}
	return 0;
}
