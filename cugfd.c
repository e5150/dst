/*
 * Copyright © 2012-2013 Lars Lindqvist
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

#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "arg.h"
#include "util/strtoid.h"

static bool dry_run = false;
static bool keep_exec = true;
static bool follow_symlinks = false;
static bool recurse = true;
static bool handle_multilinked = true;
static bool xdev = false;
static bool handle_ssh = true;
static int verbose = 1;

mode_t dir_mode = (mode_t)-1;
mode_t reg_mode = (mode_t)-1;
uid_t uid = (uid_t)-1;
gid_t gid = (gid_t)-1;
const char *user = NULL;
const char *group = NULL;

#define DO_CHMOD_DIR (dir_mode != (mode_t)(-1))
#define DO_CHMOD_REG (reg_mode != (mode_t)(-1))
#define DO_CHOWN_UID (uid != (uid_t)(-1))
#define DO_CHOWN_GID (gid != (gid_t)(-1))

#define MODE_MASK (S_ISUID|S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO)

mode_t
parse_mode_str(const char *s) {
	char *e;
	long n;
	mode_t m;

	errno = 0;
	n = strtol(s, &e, 8);
	if (*e != '\0') {
		fprintf(stderr, "%s: ERROR: non-octal mode %s\n", argv0, s);
		exit(1);
	}
	else if (errno != 0) {
		fprintf(stderr, "%s: ERROR: strtol %s: %s\n", argv0, s, strerror(errno));
		exit(1);
	}

	m = 0;

	if (n & 04000) m |= S_ISUID;
	if (n & 02000) m |= S_ISGID;
	if (n & 00400) m |= S_IRUSR;
	if (n & 00200) m |= S_IWUSR;
	if (n & 00100) m |= S_IXUSR;
	if (n & 00040) m |= S_IRGRP;
	if (n & 00020) m |= S_IWGRP;
	if (n & 00010) m |= S_IXGRP;
	if (n & 00004) m |= S_IROTH;
	if (n & 00002) m |= S_IWOTH;
	if (n & 00001) m |= S_IXOTH;

	return m;
}

int
mcn(const char *path, dev_t parentdev) {
	struct stat fs;
	int err = 0;
	bool isdir;
	bool isreg;

	if (stat(path, &fs) == -1) {
		fprintf(stderr, "%s: ERROR: stat %s: %s\n", argv0, path, strerror(errno));
		return 1;
	}

	isdir = S_ISDIR(fs.st_mode);
	isreg = S_ISREG(fs.st_mode);

	if (!follow_symlinks) {
		struct stat ls;

		if (lstat(path, &ls) == -1) {
			fprintf(stderr, "%s: ERROR: stat %s: %s\n", argv0, path, strerror(errno));
			return 1;
		}

		if (S_ISLNK(ls.st_mode)) {
			if (verbose)
				fprintf(stdout, "%s: ignoring symlink: %s\n", argv0, path);
			return 0;
		}
	}

	if (!handle_multilinked && isreg && fs.st_nlink > 1) {
		if (verbose)
			fprintf(stdout, "%s: ignoring multilinked: %s\n", argv0, path);
		return 0;
	}

	/* chown */
	if (DO_CHOWN_UID && uid != fs.st_uid || DO_CHOWN_GID && gid != fs.st_gid) {
		if (verbose)
			fprintf(stdout, "chown %s:%s '%s'\n", user ? user : "", group ? group : "", path);
		if (!dry_run) {
			err = chown(path, uid, gid);
			if (err) {
				fprintf(stderr, "%s: chown %s %d %d: %s\n", argv0, path, uid, gid, strerror(errno));
			}
		}
	}

	/* chmod */
	if (DO_CHMOD_DIR && isdir) {
		if (dir_mode != (fs.st_mode & MODE_MASK)) {
			if (verbose)
				fprintf(stdout, "chmod %04o '%s'\n", dir_mode, path);
			if (!dry_run) {
				err = chmod(path, dir_mode);
				if (err) {
					fprintf(stderr, "%s: chmod %s %o: %s\n", argv0, path, dir_mode, strerror(errno));
				}
			}
		}
	} else if (DO_CHMOD_REG && isreg) {
		mode_t new_mode = reg_mode;

		if (keep_exec) {
			if ((reg_mode & S_IRUSR) && (fs.st_mode & S_IXUSR))
				new_mode |= S_IXUSR;
			if ((reg_mode & S_IRGRP) && (fs.st_mode & S_IXGRP))
				new_mode |= S_IXGRP;
			if ((reg_mode & S_IROTH) && (fs.st_mode & S_IXOTH))
				new_mode |= S_IXOTH;
		}
		if (new_mode != (fs.st_mode & MODE_MASK)) {
			if (verbose)
				fprintf(stdout, "chmod %04o '%s'\n", new_mode, path);
			if (!dry_run) {
				err = chmod(path, new_mode);
				if (err) {
					fprintf(stderr, "%s: chmod %s %o: %s\n", argv0, path, new_mode, strerror(errno));
				}
			}
		}
	}

	/* recurse */
	if (recurse && isdir) {
		struct dirent *c;
		DIR *dp;
		char fullpath[PATH_MAX];

		if (xdev && fs.st_dev != parentdev) {
			if (verbose)
				fprintf(stdout, "%s: ignoring mountpoint: %s\n", argv0, path);
			return 0;
		}

		if ((dp = opendir(path)) == NULL) {
			fprintf(stderr, "%s: ERROR: opendir %s: %s\n", argv0, path, strerror(errno));
			return 1;
		}
		while ((c = readdir(dp))) {
			if (!strcmp(c->d_name, ".") || !strcmp(c->d_name, "..")) {
				continue;
			}
			if (!handle_ssh && !strcmp(c->d_name, ".ssh")) {
				continue;
			}
			snprintf(fullpath, PATH_MAX, "%s/%s", path, c->d_name);
			if (mcn(fullpath, parentdev)) {
				err = 1;
			}
		}
		if (closedir(dp) == -1) {
			fprintf(stderr, "%s: ERROR: closedir %s: %s\n", argv0, path, strerror(errno));
			return 1;
		}
	}
	
	return err;
}


void
usage() {
	fprintf(stderr, "usage: %s [-sxLlRKDvq] [-u <user>] [-g <group>] [-f <mode>] [-d <mode>] <path [...]>\n", argv0);
	fprintf(stderr, "\tx: don't recurse into mountpoints\n");
	fprintf(stderr, "\ts: don't touch .ssh\n");
	fprintf(stderr, "\tU: only unique files (ie nlink == 1)\n");
	fprintf(stderr, "\tK: don't honor exec-bit on files\n");
	fprintf(stderr, "\tl: follow symbolic links\n");
	fprintf(stderr, "\tD: dry run\n");
	fprintf(stderr, "\tv: more verbosity\n");
	fprintf(stderr, "\tq: quiet\n");
	exit(1);
}

int
main(int argc, char *argv[]) {
	int i = 0, ret = 0;

	ARGBEGIN {
	case 'K':
		keep_exec = false;
		break;
	case 'q':
		verbose = 0;
		break;
	case 'D':
		dry_run = true;
		break;
	case 'f':
		reg_mode = parse_mode_str(EARGF(usage()));
		break;
	case 'd':
		dir_mode = parse_mode_str(EARGF(usage()));
		break;
	case 'u':
		user = EARGF(usage());
		uid = strtouid(user);
		if (uid == (uid_t)-1)
			usage();
		break;
	case 'g':
		group = EARGF(usage());
		gid = strtogid(group);
		if (gid == (gid_t)-1)
			usage();
		break;
	case 'l':
		follow_symlinks = true;
		break;
	case 'U':
		handle_multilinked = false;
		break;
	case 'R':
		recurse = false;
		break;
	case 's':
		handle_ssh = false;
		break;
	case 'x':
		xdev = true;
		break;
	/* undocumented */
	case 'v':
		++verbose;
		break;
	default: usage();
	} ARGEND;


	if (!argc) {
		usage();
	}

	if (verbose > 2) {
		printf("user:\t%s\n",  DO_CHOWN_UID ? user  : "no change");
		printf("group:\t%s\n", DO_CHOWN_GID ? group : "no change");
		if (DO_CHMOD_REG)
			printf("file:\t%04o\n", reg_mode);
		else
			printf("file:\tno change\n");
		if (DO_CHMOD_DIR)
			printf("dir:\t%04o\n", dir_mode);
		else
			printf("dir:\tno change\n");
	}

	for (i = 0; i < argc; i++) {
		const char *path = argv[i];
		if (verbose > 1) {
			printf("would%shandle path:\t%s\n", recurse ? " recurseively " : " ",  path);
		} else {
			struct stat fs;
			if (lstat(path, &fs) == -1) {
				fprintf(stderr, "%s: ERROR: lstat %s: %s\n", argv0, path, strerror(errno));
				return 1;
			}
			if (mcn(path, fs.st_dev)) {
				ret = 1;
			}
		}
	}

	return ret;
}
