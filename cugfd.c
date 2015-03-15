/* Copyright © 2012-2013 Lars Lindqvist
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
#include <string.h>

#include "arg.h"

int dry_run = 0;
int verbose = 1;
int keep_exec = 1;

mode_t dir_mode = (mode_t)-1;
mode_t reg_mode = (mode_t)-1;
uid_t uid = (uid_t)-1;
gid_t gid = (gid_t)-1;
const char *user = NULL;
const char *group = NULL;

#define ISSET_DIR (dir_mode != (mode_t)(-1))
#define ISSET_REG (reg_mode != (mode_t)(-1))
#define ISSET_UID (uid != (uid_t)(-1))
#define ISSET_GID (gid != (gid_t)(-1))

#define MODE_MASK (S_ISUID|S_ISGID|S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO)

mode_t
parse_mode(const char *s) {
	char *e;
	long n;
	mode_t m;

	errno = 0;
	n = strtol(s, &e, 8);
	if(*e != '\0') {
		fprintf(stderr, "%s: ERROR: non-octal mode %s\n", argv0, s);
		exit(1);
	}
	else if(errno != 0) {
		fprintf(stderr, "%s: ERROR: strtol %s: %s\n", argv0, s, strerror(errno));
		exit(1);
	}

	m = 0;

	if(n & 04000) m |= S_ISUID;
	if(n & 02000) m |= S_ISGID;
	if(n & 00400) m |= S_IRUSR;
	if(n & 00200) m |= S_IWUSR;
	if(n & 00100) m |= S_IXUSR;
	if(n & 00040) m |= S_IRGRP;
	if(n & 00020) m |= S_IWGRP;
	if(n & 00010) m |= S_IXGRP;
	if(n & 00004) m |= S_IROTH;
	if(n & 00002) m |= S_IWOTH;
	if(n & 00001) m |= S_IXOTH;

	return m;
}

int
mcn(const char *path) {
	struct dirent *c;
	struct stat fs;
	int err = 0;
	DIR *dp;
	char fullpath[PATH_MAX];

	if(lstat(path, &fs) == -1) {
		fprintf(stderr, "%s: ERROR: lstat %s: %s\n", argv0, path, strerror(errno));
		return 1;
	}

	/* chown */
	if((ISSET_UID && uid != fs.st_uid) || (ISSET_GID && gid != fs.st_gid)) {
		if(verbose)
			fprintf(stdout, "chown %s:%s '%s'\n", user ? user : "", group ? group : "", path);
		if(!dry_run) {
			err = chown(path, uid, gid);
			if(err) {
				fprintf(stderr, "%s: chown %s %d %d: %s\n", argv0, path, uid, gid, strerror(errno));
			}
		}
	}

	/* chmod */
	if(ISSET_DIR && S_ISDIR(fs.st_mode)) {
		if(dir_mode != (fs.st_mode & MODE_MASK)) {
			if(verbose)
				fprintf(stdout, "chmod %04o '%s'\n", dir_mode, path);
			if(!dry_run) {
				err = chmod(path, dir_mode);
				if(err) {
					fprintf(stderr, "%s: chmod %s %o: %s\n", argv0, path, dir_mode, strerror(errno));
				}
			}
		}
	} else if(ISSET_REG && S_ISREG(fs.st_mode)) {
		mode_t new_mode = reg_mode;

		if(keep_exec) {
			if((reg_mode & S_IRUSR) && (fs.st_mode & S_IXUSR))
				new_mode |= S_IXUSR;
			if((reg_mode & S_IRGRP) && (fs.st_mode & S_IXGRP))
				new_mode |= S_IXGRP;
			if((reg_mode & S_IROTH) && (fs.st_mode & S_IXOTH))
				new_mode |= S_IXOTH;
		}
		if(new_mode != (fs.st_mode & MODE_MASK)) {
			if(verbose)
				fprintf(stdout, "chmod %04o '%s'\n", new_mode, path);
			if(!dry_run) {
				err = chmod(path, new_mode);
				if(err) {
					fprintf(stderr, "%s: chmod %s %o: %s\n", argv0, path, new_mode, strerror(errno));
				}
			}
		}
	}

	/* recurse */
	if(S_ISDIR(fs.st_mode)) {
		if((dp = opendir(path)) == NULL) {
			fprintf(stderr, "%s: ERROR: opendir %s: %s\n", argv0, path, strerror(errno));
			return 1;
		}
		while((c = readdir(dp))) {
			if(!strcmp(c->d_name, ".") || !strcmp(c->d_name, ".."))
				continue;
			snprintf(fullpath, PATH_MAX, "%s/%s", path, c->d_name);
			if(!mcn(fullpath))
				return 0;
		}
		if(closedir(dp) == -1) {
			fprintf(stderr, "%s: ERROR: closedir %s: %s\n", argv0, path, strerror(errno));
			return 1;
		}
	}
	
	return 1;
}


void
parse_uidstr(const char *str) {
	struct passwd *pw = NULL;

	errno = 0;
	user = str;
	pw = getpwnam(user);
	if(errno) {
		fprintf(stderr, "%s: ERROR: getpwnam %s: %s\n", argv0, user, strerror(errno));
		exit(2);
	} else if(!pw) {
		fprintf(stderr, "%s: ERROR: No such user: %s\n", argv0, user);
		exit(2);
	}
	uid = pw->pw_uid;

}

void
parse_gidstr(const char *str) {
	struct group *gr = NULL;

	errno = 0;
	group = str;
	gr = getgrnam(group);
	if(errno) {
		fprintf(stderr, "%s: ERROR: getgrnam %s: %s\n", argv0, group, strerror(errno));
		exit(2);
	} else if(!gr) {
		fprintf(stderr, "%s: ERROR: No such group: %s\n", argv0, group);
		exit(2);
	}
	gid = gr->gr_gid;

}

void
usage() {
	fprintf(stderr, "usage: %s [-KDq] [-u <user>] [-g <group>] [-f <mode>] [-d <mode>] <path [...]>\n", argv0);
	exit(1);
}

int
main(int argc, char *argv[]) {
	int i = 0;

	ARGBEGIN {
	case 'K':
		keep_exec = 0;
		break;
	case 'q':
		verbose = 0;
		break;
	case 'D':
		dry_run = 1;
		break;
	case 'f':
		reg_mode = parse_mode(EARGF(usage()));
		break;
	case 'd':
		dir_mode = parse_mode(EARGF(usage()));
		break;
	case 'u':
		parse_uidstr(EARGF(usage()));
		break;
	case 'g':
		parse_gidstr(EARGF(usage()));
		break;
	/* undocumented */
	case 'v':
		++verbose;
		break;
	default: usage();
	} ARGEND;


	if(!argc) {
		usage();
	}

	if(verbose > 1) {
		printf("user:\t%s\n",  ISSET_UID ? user  : "no change");
		printf("group:\t%s\n", ISSET_GID ? group : "no change");
		if(ISSET_REG)
			printf("file:\t%04o\n", reg_mode);
		else
			printf("file:\tno change\n");
		if(ISSET_DIR)
			printf("dir:\t%04o\n", dir_mode);
		else
			printf("dir:\tno change\n");
	}

	for(i = 0; i < argc; i++) {
		if(verbose > 1)
			printf("path:\t%s\n", argv[i]);
		else
			mcn(argv[i]);
	}
	return 0;
}
