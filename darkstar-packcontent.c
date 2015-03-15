/* Copyright © 2012-2015 Lars Lindqvist
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
#include <errno.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "util.h"

void
usage() {
	fprintf(stderr, "usage: %s -s </var/log/package/file [...]>\n", argv0);
	fprintf(stderr, "usage: %s [-v] [-d] [-p|-c|-e|-m] </var/log/package/file [...]>\n", argv0);
	exit(1);
}

static int ignore_dirs = 1;
static int verbose = 0;

enum { M_SIZE,
       M_FL_PRINT,
       M_FL_EXISTS,
       M_FL_MISSING,
       M_FL_CHECK
} mode = M_FL_PRINT;

int
read_file(const char *pkgfile) {
	FILE *fp;
	char *buf;
	char *pkg_base;
	off_t size = 0;
	struct stat fs;
	ino_t *inodes = NULL;
	size_t inodes_n;

	if(!(fp = fopen(pkgfile, "r"))) {
		fprintf(stderr, "%s: ERROR: fopen %s: %s\n", argv0, pkgfile, strerror(errno));
		return 1;
	}

	buf = calloc(PATH_MAX, sizeof(char));

	if (mode == M_SIZE) {
		inodes = malloc(sizeof(ino_t));
		inodes_n = 0;
	}

	while(fgets(buf, PATH_MAX, fp)) {
		if(!strcmp(buf, "FILE LIST:\n")) {
			break;
		}
	}

	if(fgets(buf, PATH_MAX, fp) && strcmp(buf, "./\n")) {
		fprintf(stderr, "%s: %s does not look like a package file\n", argv0, pkgfile);
		goto end;
	}

	pkg_base = dst_packname(pkgfile, SPKG_BASE);

	buf[0] = '/';
	
	while(fgets(buf + 1, PATH_MAX - 1, fp)) {
		char *c;
		char *rp = NULL;
		char *path;
		int found;

		if(strstr(buf, "/install") == buf)
			continue;

		if((c = strstr(buf, ".new\n")) || (c = strrchr(buf, '\n'))) {
			*c = '\0';
		} else {
			fprintf(stderr, "%s: CANTHAPPEN\n", argv0);
			exit(1);
		}

		if(ignore_dirs && buf[strlen(buf) - 1] == '/') {
			continue;
		}

		if((c = strstr(buf, "/lib64/incoming/")) == buf
		|| (c = strstr(buf, "/lib/incoming/")) == buf) {
			size_t len = strlen("/incoming");
			c = strstr(buf, "/incoming/");
			memmove(c, c + len, strlen(c + len) + 1);
		}

		found = !access(buf, F_OK);

		if (found && !(rp = realpath(buf, NULL))) {
			fprintf(stderr, "%s: ERROR: realpath %s: %s\n", argv0, buf, strerror(errno));
			continue;
		}

		path = rp ? rp : buf;

		if(mode == M_FL_PRINT
		||(mode == M_FL_EXISTS && found)
		||(mode == M_FL_MISSING && !found)) {
			if(verbose)
				fprintf(stdout, "%s:", pkg_base);
			fprintf(stdout, "%s\n", path);
		} else if(mode == M_FL_CHECK) {
			if(verbose)
				fprintf(stdout, "%s:", pkg_base);
			fprintf(stdout, "%s:%s\n", path, found ? "present" : "missing"); 
		} else if(mode == M_SIZE && found) {
			if(!stat(path, &fs)) {
				size_t i;

				for (i = 0; i < inodes_n; ++i) {
					if (inodes[i] == fs.st_ino)
						break;
				}
				if (i < inodes_n) {
					if (rp)
						free(rp);
					continue;
				}
				inodes = realloc(inodes, ++inodes_n * sizeof(ino_t));
				inodes[inodes_n - 1] = fs.st_ino;

				size += fs.st_size;
			} else {
				fprintf(stderr, "%s: ERROR: stat %s: %s\n", argv0, path, strerror(errno));
			}
		}

		if (rp)
			free(rp);
	}
	if(mode == M_SIZE) {
		char *s = hr(size);
		fprintf(stdout, "%s\t%s\n", s, pkg_base);
		free(s);
		free(inodes);
	}

	free(pkg_base);
end:
	free(buf);
	fclose(fp);
	return 0;
}


int
main(int argc, char *argv[]) {
	int i, ret;
	char **files;

	ARGBEGIN {
	case 'e':
		mode = M_FL_EXISTS;
		break;
	case 'm':
		mode = M_FL_MISSING;
		break;
	case 's':
		mode = M_SIZE;
		ignore_dirs = 1;
		break;
	case 'd':
		ignore_dirs = 0;
		break;
	case 'p':
		mode = M_FL_PRINT;
		break;
	case 'c':
		mode = M_FL_CHECK;
		break;
	case 'v':
		++verbose;
		break;
	default:
		usage();
	} ARGEND;

	if(!argc)
		usage();

	ret = 0;

	files = dst_findpkg(argc, argv);

	for(i = 0; i < argc; ++i) {
		int tmp;

		if (!files[i]) {
			fprintf(stderr, "%s: ERROR: %s: No such package\n", argv0, argv[i]);
			continue;
		}

		tmp = read_file(files[i]);
		if(tmp > ret)
			ret = tmp;

		free(files[i]);
	}
	
	free(files);

	return ret;
}
