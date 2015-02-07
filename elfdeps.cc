/*
 * Copyright © 2013-2015 Lars Lindqvist
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

/* Requires: https://github.com/serge1/ELFIO */

#include <iostream>
#include <vector>
#include <elfio/elfio.hpp>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

extern "C" {
	#include "util.h"
}

const char *LIB64DIRS[] = {
	"/usr/local/lib64",
	"/usr/lib64",
	"/lib64",
	NULL,
};
const char *LIB32DIRS[] = {
	"/usr/local/lib",
	"/usr/lib",
	"/lib",
	NULL,
};

struct Sofile {
	char *path;
	struct Sofile *next;
};

struct Libdir {
	char *path;
	struct Sofile *head;
	struct Libdir *next;
};

struct Elffile {
	const char *path;
	int elfclass;
	std::vector<std::string> needed;
	std::vector<std::string> rpaths;
};

struct Libdir *lib64dirs = NULL;
struct Libdir *lib32dirs = NULL;
struct Libdir *librpaths = NULL;

enum { V_QUIET = 0, V_PROPER, V_VERBOSE, V_DEBUG, V_REALLY_DEBUG };
static int verbosity = V_PROPER;
enum { EE_NONE = 0, EE_READ, EE_MISSING, EE_NONELF };

int
read_libdir(struct Libdir **head, const char *path) {
	struct dirent *dc;
	DIR *dp;
	struct Libdir *libdir = NULL;
	struct Sofile *sofile = NULL;

	if(verbosity >= V_VERBOSE)
		fprintf(stderr, "%s: %s/: listing\n", argv0, path);

	if(!(dp = opendir(path))) {
		fprintf(stderr, "%s: %s/: ERROR: opendir: ", argv0, path);
		perror(NULL);
		return 1;
	}

	libdir = (struct Libdir*)malloc(sizeof(struct Libdir));
	libdir->path = strdup(path);
	libdir->head = NULL;

	while((dc = readdir(dp))) {
		if(!strstr(dc->d_name, ".so")) {
			if(verbosity >= V_REALLY_DEBUG)
				fprintf(stderr, "%s: %s/%s: skipping\n", argv0, path, dc->d_name);

			continue;
		} else {
			if(verbosity >= V_REALLY_DEBUG)
				fprintf(stderr, "%s: %s/%s: adding\n", argv0, path, dc->d_name);
		}

		sofile = (struct Sofile*)malloc(sizeof(struct Sofile));
		sofile->path = strdup(dc->d_name);

		sofile->next = libdir->head;
		libdir->head = sofile;
	}

	closedir(dp);

	libdir->next = *head;
	*head = libdir;

	return 0;
}

int
libdir_is_read(const char *path, struct Libdir *head) {
	struct Libdir *tmp;
	size_t len = strlen(path);

	for(tmp = head; tmp; tmp = tmp->next) {
		if(!strncmp(tmp->path, path, len + 1)) {
			if(verbosity >= V_DEBUG)
				fprintf(stderr, "%s: %s/: skipping, already read\n", argv0, path);
			return 1;
		}
	}

	return 0;
}

std::string
read_rpath(const char *rpath, struct Elffile *elffile) {
	char path[PATH_MAX];
	char *c;
	char *respath;

	memset(path, '\0', PATH_MAX);

	if(strstr(rpath, "$ORIGIN") == rpath) {
		strncpy(path, elffile->path, PATH_MAX - 1);
		
		if(!(c = strrchr(path, '/'))) {
			strcpy(path, "./");
			c = path + 1;
		}

		strncpy(c, rpath + strlen("$ORIGIN"), PATH_MAX - (c - path) - 1);
	} else {
		strncpy(path, rpath, PATH_MAX - 1);
	}

	if(!(respath = realpath(path, NULL))) {
		fprintf(stderr, "%s: %s: could not resolve rpath %s: ", argv0, elffile->path, path);
		perror(NULL);
		return "";
	}
	strncpy(path, respath, PATH_MAX - 1);
	free(respath);

	if(verbosity >= V_DEBUG)
		fprintf(stderr, "%s: %s: using rpath: %s\n", argv0, elffile->path, path);


	if(!libdir_is_read(path, librpaths)) {
		if(read_libdir(&librpaths, path) != 0)
			return "";
	}
	if(verbosity >= V_VERBOSE) {
		int r32 = libdir_is_read(path, lib32dirs);
		int r64 = libdir_is_read(path, lib64dirs);
		if(r32 || r64)
			fprintf(stderr, "%s: %s: WARNING: redundant RPATH: %s\n",            argv0, elffile->path, path);
		if(elffile->elfclass == ELFCLASS32 && r64)
			fprintf(stderr, "%s: %s: WARNING: 32-bit RPATH %s in 64-bit file\n", argv0, elffile->path, path);
		if(elffile->elfclass == ELFCLASS64 && r32)
			fprintf(stderr, "%s: %s: WARNING: 64-bit RPATH %s in 32-bit file\n", argv0, elffile->path, path);
	}

	return path;
}

int
search_in_libdir(const char *lib, struct Libdir *dir, struct Elffile *elffile) {
	struct Sofile *tmp;
	size_t len;

	if(!dir) {
		fprintf(stderr, "%s: FATAL: CANTHAPPEN\n", argv0);
		exit(1);
	}

	if(verbosity >= V_DEBUG)
		fprintf(stderr, "%s: %s: %s/: searching for %s\n", argv0, elffile->path, dir->path, lib);

	len = strlen(lib);

	for(tmp = dir->head; tmp; tmp = tmp->next) {
		if(verbosity >= V_REALLY_DEBUG)
			fprintf(stderr, "%s: %s: %s/: comparing %s to %s\n", argv0, elffile->path, dir->path, lib, tmp->path);

		if(!strncmp(tmp->path, lib, len + 1)) {
			return 1;
		}
	}

	return 0;
}

int
search(struct Elffile *elffile) {
	std::vector<std::string>::iterator n_it;
	std::vector<std::string>::iterator r_it;
	struct Libdir *tmp;
	const char *dir, *lib;
	const char *found_dir;
	size_t len;
	size_t missing = 0;

	for(n_it = elffile->needed.begin(); n_it != elffile->needed.end(); ++n_it) {
		found_dir = NULL;
		lib = (*n_it).c_str();

		if(verbosity >= V_VERBOSE)
			fprintf(stderr, "%s: %s: searching for %s\n", argv0, elffile->path, lib);

		for(r_it = elffile->rpaths.begin();
		    r_it != elffile->rpaths.end() && !found_dir;
		    ++r_it)
		{
			dir = (*r_it).c_str();
			len = strlen(dir);

			for(tmp = librpaths; tmp; tmp = tmp->next) {
				if(!strncmp(tmp->path, dir, len + 1)) {
					break;
				}
			}
			if(search_in_libdir(lib, tmp, elffile)) {
				found_dir = tmp->path;

			}
		}
		for(tmp = elffile->elfclass == ELFCLASS32 ? lib32dirs : lib64dirs;
		    tmp && !found_dir;
		    tmp = tmp->next)
		{
			if(search_in_libdir(lib, tmp, elffile)) {
				found_dir = tmp->path;
			}
		}

		if(found_dir) {
			fprintf(stdout, "%s:%s:%s/%s\n", elffile->path, lib, found_dir, lib);
		} else {
			++missing;
			fprintf(stdout, "%s:%s:missing\n", elffile->path, lib);
		}
	}
	if(verbosity >= V_VERBOSE) {
		if(missing)
			fprintf(stderr, "%s: %s: WARNING: missing %lu\n", argv0, elffile->path, missing);
		else
			fprintf(stderr, "%s: %s: OK\n", argv0, elffile->path);
	}

	if(missing > 0)
		return 1;
	else
		return 0;
}

int
read_elf(const char *path) {
	ELFIO::elfio elf;
	ELFIO::Elf_Half n, i, j;
	ELFIO::section *sec;
	uint64_t m, tag, val;
	std::string str;
	struct Elffile elffile;
	struct Libdir **libdir;
	const char **syslib;

	if(verbosity >= V_VERBOSE) {
		fprintf(stderr, "\n");
	}

	if(!elf.load(path)) {
		FILE *fp = fopen(path, "rb");
		if(fp)
			fclose(fp);
		if(errno) {
			fprintf(stderr, "%s: %s: ERROR: ", argv0, path);
			perror(NULL);
			return EE_READ;
		} else {
			/* probably not elf */
			if (verbosity >= V_VERBOSE)
				fprintf(stderr, "%s: %s: skipping\n", argv0, path);
			return EE_NONELF;
		}
	}

	elffile.path = path;
	elffile.elfclass = elf.get_class();

	switch(elffile.elfclass) {
	case ELFCLASS32:
		libdir = &lib32dirs;
		syslib = LIB32DIRS;
		break;
	case ELFCLASS64:
		libdir = &lib64dirs;
		syslib = LIB64DIRS;
		break;
	default:
		/* CANTHAPPEN? */
		fprintf(stderr, "%s: ERROR: Unknown ELF class %s: ", argv0, path);
		perror(NULL);
		return EE_NONELF;
	}

	if(verbosity >= V_VERBOSE)
		fprintf(stderr, "%s: %s: reading ELF%d file\n", argv0, path,
		        elffile.elfclass == ELFCLASS32 ? 32 : 64);

	if(!*libdir) {
		for(i = 0; syslib[i] != NULL; ++i) {
			read_libdir(libdir, syslib[i]);
		}
	}

	n = elf.sections.size();
	for(i = 0; i < n; ++i) {
		sec = elf.sections[i];
		if(sec->get_type() == SHT_DYNAMIC) {
			ELFIO::dynamic_section_accessor dyn(elf, sec);

			m = dyn.get_entries_num();
			for(j = 0; j < m; ++j) {
				tag = 0;
				val = 0;
				dyn.get_entry(j, tag, val, str);
				if(tag == DT_RPATH) {
					char *s = strdup(str.c_str());
					char *tok = strtok(s, ":");

					if(verbosity >= V_DEBUG)
						fprintf(stderr, "%s: %s: RPATH=%s\n", argv0, path, s);

					if(!tok)
						tok = (char*)str.c_str();

					do {
						std::string abs = read_rpath(tok, &elffile);
						if(!abs.empty()) {
							elffile.rpaths.push_back(abs);
						}
					} while((tok = strtok(NULL, ":")));

					free(s);

				}
				else if(tag == DT_NEEDED) {
					if(verbosity >= V_DEBUG)
						fprintf(stderr, "%s: %s: NEEDED=%s\n", argv0, path, str.c_str());

					elffile.needed.push_back(str);
				}
				else if(tag == DT_NULL)
					break;
			}
		}
	}

	if(search(&elffile) != 0)
		return EE_MISSING;
	else
		return EE_NONE;
}

void
usage() {
	fprintf(stderr, "usage: [-v|-q] %s <file [...]>\n", argv0);
	exit(1);
}

void
cleanup(struct Libdir *ldhead) {
	struct Libdir *libdir = NULL;
	struct Sofile *sofile = NULL;
	struct Sofile *sohead;

	while(ldhead) {
		if(verbosity >= V_DEBUG)
			fprintf(stderr, "%s: %s/: cleaning up\n", argv0, ldhead->path);

		libdir = ldhead->next;
		sohead = ldhead->head;
		while(sohead) {
			sofile = sohead->next;
			free(sohead->path);
			free(sohead);
			sohead = sofile; 
		}
		free(ldhead->path);
		free(ldhead);

		ldhead = libdir;
	}
}



int
main(int argc, char *argv[]) {
	size_t i;
	int ret = EE_NONE;

	ARGBEGIN {
	case 'v':
		++verbosity;
		break;
	/* undocumented */
	case 'q':
		--verbosity;
		break;
	default:
		usage();
	} ARGEND;

	if(!argc || verbosity < V_QUIET)
		usage();

	if(verbosity >= V_VERBOSE) {
		setvbuf(stdout, NULL, _IONBF, 0);
	}

	for(i = 0; i < (size_t)argc; ++i) {
		int tmp = read_elf(argv[i]);
		if(tmp > ret) {
			if(tmp == EE_NONELF && verbosity < V_PROPER) {
				/* don't whine about non elf */
			} else {
				ret = tmp;
			}
		}
	
	}

	cleanup(lib32dirs);
	cleanup(lib64dirs);
	cleanup(librpaths);

	return ret;
}
