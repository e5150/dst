/*
 * Copyright © 2011, Brian Raiter <breadbox@muppetlabs.com>
 * Copyright © 2012-2013, 2015, 2016 Lars Lindqvist
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * The part of the code that does the ELF parsing is based on
 * elfls / elfrw of ELFkickers by Brian Raiter, which can be found
 * at https://github.com/BR903/ELFkickers
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <elf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>

#include "util/slist.h"
#include "util/admdir.h"
#include "util/ealloc.h"
#include "arg.h"

enum ec_t { EC_32, EC_64, EC_RP };

static struct {
	enum ec_t ec;
	const char *path;
} default_libdirs[] = {
	{ EC_64, "/usr/local/lib64" },
	{ EC_32, "/usr/local/lib" },
	{ EC_64, "/lib64" },
	{ EC_64, "/usr/lib64" },
	{ EC_32, "/usr/lib" },
	{ EC_32, "/lib" },
};

struct libdir_t {
	char *path;
	enum ec_t ec;
	struct slist_t list;
	struct libdir_t *next;
};

struct elffile_t {
	char *path;
	char *pkg;
	enum ec_t ec;
	struct slist_t needed;
	struct slist_t rpaths;
	struct elffile_t *next;
};

#define PR_ELF_ONLY 0
#define PR_OWN_PACK (1 << 0)
#define PR_OWN_PATH (1 << 1)
#define PR_DTNEEDED (1 << 2)
#define PR_DEP_PATH (1 << 3)
#define PR_DEP_PACK (1 << 4)
static unsigned verbose;

static const char missing_str[] = "missing";
static bool find_own_pkg = false;
static bool find_dep_pkg = false;
static bool no_circular = false;
static bool recurse = false;
static bool only_missing = false;
static struct {
	bool use;
	regex_t rex;
	const char *str;
} who_needs = { false };

static struct elffile_t *head = NULL;


static int elfrw_initialize_ident(unsigned char const *e_ident);

static unsigned char _elfrw_native_data;
static unsigned char _elfrw_current_class;
static unsigned char _elfrw_current_data;

#define native_form() (_elfrw_native_data == _elfrw_current_data)
#define is64bit_form() (_elfrw_current_class == ELFCLASS64)

static Elf32_Half
rev_32half(Elf32_Half in) {
	return ((in & 0x00FF) << 8) | ((in & 0xFF00) >> 8);
}
static Elf32_Word
rev_32word(Elf32_Word in) {
#if defined __i386__ || defined __x86_64__
	__asm__("bswap %0" : "=r"(in) : "0"(in));
	return in;
#else
	return ((in & 0x000000FFU) << 24)
	 | ((in & 0x0000FF00U) << 8)
	 | ((in & 0x00FF0000U) >> 8)
	 | ((in & 0xFF000000U) >> 24);
#endif
}

static inline void
revinplc2(void *in) {
	unsigned char tmp;

	tmp = ((unsigned char *)in)[0];
	((unsigned char *)in)[0] = ((unsigned char *)in)[1];
	((unsigned char *)in)[1] = tmp;
}

static inline void
revinplc4(void *in) {
#if defined __i386__ || defined __x86_64__
	__asm__("bswap %0" : "=r"(*(unsigned int*)in) : "0"(*(unsigned int*)in));
#else
	unsigned char tmp;

	tmp = ((unsigned char*)in)[0];
	((unsigned char*)in)[0] = ((unsigned char*)in)[3];
	((unsigned char*)in)[3] = tmp;
	tmp = ((unsigned char*)in)[1];
	((unsigned char*)in)[1] = ((unsigned char*)in)[2];
	((unsigned char*)in)[2] = tmp;
#endif
}

static inline void
revinplc8(void *in) {
#if defined __x86_64__
	__asm__("bswap %0" : "=r"(*(unsigned long*)in) : "0"(*(unsigned long*)in));
#else
	unsigned char tmp;

	tmp = ((unsigned char*)in)[0];
	((unsigned char*)in)[0] = ((unsigned char*)in)[7];
	((unsigned char*)in)[7] = tmp;
	tmp = ((unsigned char*)in)[1];
	((unsigned char*)in)[1] = ((unsigned char*)in)[6];
	((unsigned char*)in)[6] = tmp;
	tmp = ((unsigned char*)in)[2];
	((unsigned char*)in)[2] = ((unsigned char*)in)[5];
	((unsigned char*)in)[5] = tmp;
	tmp = ((unsigned char*)in)[3];
	((unsigned char*)in)[3] = ((unsigned char*)in)[4];
	((unsigned char*)in)[4] = tmp;
#endif
}

#define revinplc_32half(in)  (revinplc2(in))
#define revinplc_64half(in)  (revinplc2(in))
#define revinplc_64word(in)  (revinplc4(in))
#define revinplc_64xword(in) (revinplc8(in))

static int
elfrw_initialize_ident(unsigned char const *ident) {
	unsigned char class, data, version;
	if (ident[EI_MAG0] != ELFMAG0 || ident[EI_MAG1] != ELFMAG1
	 || ident[EI_MAG2] != ELFMAG2 || ident[EI_MAG3] != ELFMAG3)
		return -1;
	class = ident[EI_CLASS];
	data = ident[EI_DATA];
	version = ident[EI_VERSION];

	if (!_elfrw_native_data) {
		int msb = 1;
		*(char *)&msb = 0;
		_elfrw_native_data = msb ? ELFDATA2MSB : ELFDATA2LSB;
	}

	switch (class) {
	case ELFCLASS32:
		_elfrw_current_class = ELFCLASS32;
		break;
	case ELFCLASS64:
		_elfrw_current_class = ELFCLASS64;
		break;
	default:
		return -EI_CLASS;
	}

	switch (data) {
	case ELFDATA2LSB:
		_elfrw_current_data = ELFDATA2LSB;
		break;
	case ELFDATA2MSB:
		_elfrw_current_data = ELFDATA2MSB;
		break;
	default:
		return -EI_DATA;
	}

	if (version != EV_CURRENT)
		return -EI_VERSION;

	return 0;
}

static int
elfrw_read_Ehdr(FILE *fp, Elf64_Ehdr *in) {

	if (1 != fread(in->e_ident, EI_NIDENT, 1, fp))
		return -1;
	if (elfrw_initialize_ident(in->e_ident) < 0)
		return -1;

	if (is64bit_form()) {
		if (1 != fread((char *)in + EI_NIDENT, sizeof *in - EI_NIDENT, 1, fp))
			return -1;
		if (!native_form()) {
			revinplc_64half(&in->e_type);
			revinplc_64half(&in->e_machine);
			revinplc_64word(&in->e_version);
			revinplc_64xword(&in->e_entry);
			revinplc_64xword(&in->e_phoff);
			revinplc_64xword(&in->e_shoff);
			revinplc_64word(&in->e_flags);
			revinplc_64half(&in->e_ehsize);
			revinplc_64half(&in->e_phentsize);
			revinplc_64half(&in->e_phnum);
			revinplc_64half(&in->e_shentsize);
			revinplc_64half(&in->e_shnum);
			revinplc_64half(&in->e_shstrndx);
		}
	} else {
		Elf32_Ehdr in32;
		if (1 != fread((char *)&in32 + EI_NIDENT, sizeof in32 - EI_NIDENT, 1, fp))
			return -1;
		if (native_form()) {
			in->e_type = in32.e_type;
			in->e_machine = in32.e_machine;
			in->e_version = in32.e_version;
			in->e_entry = in32.e_entry;
			in->e_phoff = in32.e_phoff;
			in->e_shoff = in32.e_shoff;
			in->e_flags = in32.e_flags;
			in->e_ehsize = in32.e_ehsize;
			in->e_phentsize = in32.e_phentsize;
			in->e_phnum = in32.e_phnum;
			in->e_shentsize = in32.e_shentsize;
			in->e_shnum = in32.e_shnum;
			in->e_shstrndx = in32.e_shstrndx;
		} else {
			in->e_type = rev_32half(in32.e_type);
			in->e_machine = rev_32half(in32.e_machine);
			in->e_version = rev_32word(in32.e_version);
			in->e_entry = rev_32word(in32.e_entry);
			in->e_phoff = rev_32word(in32.e_phoff);
			in->e_shoff = rev_32word(in32.e_shoff);
			in->e_flags = rev_32word(in32.e_flags);
			in->e_ehsize = rev_32half(in32.e_ehsize);
			in->e_phentsize = rev_32half(in32.e_phentsize);
			in->e_phnum = rev_32half(in32.e_phnum);
			in->e_shentsize = rev_32half(in32.e_shentsize);
			in->e_shnum = rev_32half(in32.e_shnum);
			in->e_shstrndx = rev_32half(in32.e_shstrndx);
		}
	}
	return 0;
}

static int
elfrw_read_Phdrs(FILE *fp, Elf64_Phdr *in, int count) {
	int i;

	for (i = 0; i < count; ++i, ++in) {
		if (is64bit_form()) {
			if (1 != fread(in, sizeof(Elf64_Phdr), 1, fp))
				break;
			if (!native_form()) {
				revinplc_64word(&in->p_type);
				revinplc_64word(&in->p_flags);
				revinplc_64xword(&in->p_offset);
				revinplc_64xword(&in->p_vaddr);
				revinplc_64xword(&in->p_paddr);
				revinplc_64xword(&in->p_filesz);
				revinplc_64xword(&in->p_memsz);
				revinplc_64xword(&in->p_align);
			}
		} else {
			Elf32_Phdr in32;
			if (1 != fread(&in32, sizeof(Elf32_Phdr), 1, fp))
				break;
			if (native_form()) {
				in->p_type = in32.p_type;
				in->p_flags = in32.p_flags;
				in->p_offset = in32.p_offset;
				in->p_vaddr = in32.p_vaddr;
				in->p_paddr = in32.p_paddr;
				in->p_filesz = in32.p_filesz;
				in->p_memsz = in32.p_memsz;
				in->p_align = in32.p_align;
			} else {
				in->p_type = rev_32word(in32.p_type);
				in->p_offset = rev_32word(in32.p_offset);
				in->p_vaddr = rev_32word(in32.p_vaddr);
				in->p_paddr = rev_32word(in32.p_paddr);
				in->p_filesz = rev_32word(in32.p_filesz);
				in->p_memsz = rev_32word(in32.p_memsz);
				in->p_flags = rev_32word(in32.p_flags);
				in->p_align = rev_32word(in32.p_align);
			}
		}

	}
	return i;
}

static void *
getarea(FILE *fp, long offset, unsigned long size) {
	void *buf;

	if (fseek(fp, offset, SEEK_SET))
		return NULL;
	buf = emalloc(size);
	if (fread(buf, size, 1, fp) != 1) {
		free(buf);
		return NULL;
	}
	return buf;
}

static int
readelfhdr(FILE *fp, Elf64_Ehdr *elffhdr) {
	const unsigned char *id = elffhdr->e_ident;

	if (elfrw_read_Ehdr(fp, elffhdr) < 0) {
		return -1;
	}
	if (memcmp(id, ELFMAG, SELFMAG)
	|| (id[EI_CLASS] != ELFCLASS32 && id[EI_CLASS] != ELFCLASS64)
	|| (id[EI_DATA] != ELFDATA2MSB && id[EI_DATA] != ELFDATA2LSB)
	|| (id[EI_VERSION] != EV_CURRENT))
		return -1;

	switch (elffhdr->e_type) {
	case ET_EXEC:
	case ET_DYN:
		break;
	case ET_CORE:
	case ET_REL:
	default:
		return -1;
	}
	if (elffhdr->e_version != EV_CURRENT)
		return -1;
	if (elffhdr->e_ehsize != sizeof(Elf32_Ehdr) && elffhdr->e_ehsize != sizeof(Elf64_Ehdr))
		return -1;

	if (elffhdr->e_phoff != 0
	 && elffhdr->e_phentsize != sizeof(Elf32_Phdr)
	 && elffhdr->e_phentsize != sizeof(Elf64_Phdr))
		return -1;
	if (elffhdr->e_shoff != 0
	 && elffhdr->e_shentsize != sizeof(Elf32_Shdr)
	 && elffhdr->e_shentsize != sizeof(Elf64_Shdr))
		return -1;

	return 0;
}

static void
cleanup_libdir(struct libdir_t *ldhead) {
	struct libdir_t *libdir = NULL;

	while (ldhead) {

		libdir = ldhead->next;
		cleanup_slist(&ldhead->list);
		free(ldhead->path);
		free(ldhead);

		ldhead = libdir;
	}
}

static struct libdir_t *
mk_libdir(const char *path, enum ec_t ec) {
	struct libdir_t *libdir = NULL;
	struct dirent *dc;
	DIR *dp;

	if (!(dp = opendir(path))) {
		fprintf(stderr, "%s: ERROR: opendir %s %s\n", argv0, path, strerror(errno));
		return NULL;
	}

	libdir = (struct libdir_t *)ecalloc(1, sizeof(struct libdir_t));
	libdir->path = strdup(path);

	while ((dc = readdir(dp))) {
		if (!strstr(dc->d_name, ".so")) {
			continue;
		}

		add_to_slist(&libdir->list, strdup(dc->d_name), NULL, 100);
	}

	if (closedir(dp) == -1) {
		fprintf(stderr, "%s: ERROR: closedir %s: %s\n", argv0, path, strerror(errno));
	}

	sort_slist(&libdir->list);

	libdir->ec = ec;

	return libdir;
}

static bool
matches_rpath(const char *path, const struct slist_t *rpaths) {
	size_t len = strlen(path);
	size_t i;

	for (i = 0; i < rpaths->i; ++i) {
		int cmp = strncmp(path, rpaths->items[i].str, len);

		if (!cmp) {
			return true;
		}
	}
	return false;
}

static bool
find_needed(const char *path, char *ret, const struct elffile_t *elf, const struct libdir_t *libdirs) {
	const struct libdir_t *libdir;

	for (libdir = libdirs; libdir; libdir = libdir->next) {
		const char *dirpath = libdir->path;
		const struct slist_item_t *match;

		if (libdir->ec == EC_RP) {
			if (!matches_rpath(dirpath, &elf->rpaths)) {
				continue;
			}
		} else if (libdir->ec != elf->ec) {
			continue;
		}
		
		if ((match = bsearch_slist(path, &libdir->list))) {
			char full[PATH_MAX];
			snprintf(full, PATH_MAX - 1, "%s/%s", dirpath, match->str);
			full[PATH_MAX - 1] = '\0';
			realpath(full, ret);
			return true;
		}
		
	}
	
	sprintf(ret, missing_str);
	return false;
}

static struct libdir_t *
init_sysdirs() {
	struct libdir_t *head = NULL;
	size_t i;

	for (i = 0; i < sizeof(default_libdirs) / sizeof(default_libdirs[0]); ++i) {
		enum ec_t dir_ec = default_libdirs[i].ec;
		const char *path = default_libdirs[i].path;
		struct libdir_t *tmp = mk_libdir(path, dir_ec);

		if (tmp) {
			tmp->next = head;
			head = tmp;
		}
	}
	return head;
}

static struct libdir_t *
add_rpath(const char *path, struct libdir_t *head) {
	struct libdir_t *tmp;

	for (tmp = head; tmp; tmp = tmp->next) {
		if (!strcmp(tmp->path, path))
			return head;
	}
	if ((tmp = mk_libdir(path, EC_RP))) {
		tmp->next = head;
		head = tmp;
	}
	return head;
}

static void
add_tok_to_line(char **line, size_t *len, const char *tok) {
	size_t new = *len + 1 + strlen(tok) + 1;

	*line = erealloc(*line, new);
	*len += sprintf(*line + *len, "%s:", tok);
}

static int
handle(struct elffile_t *head, const struct slist_t *fslist) {
	struct slist_t *lines;
	struct libdir_t *libdirs = NULL;
	struct elffile_t *elf;
	int err = 0;
	size_t i;

	if (verbose == PR_ELF_ONLY) {
		for (elf = head; elf; elf = elf->next) {
			for (i = 0; i < elf->rpaths.i; ++i) {
				if (find_own_pkg)
					printf("%s:", elf->pkg);
				printf("%s:", elf->path);
				printf("rpath:%s\n", elf->rpaths.items[i].str);
			}
			for (i = 0; i < elf->needed.i; ++i) {
				if (find_own_pkg)
					printf("%s:", elf->pkg);
				printf("%s:", elf->path);
				printf("needed:%s\n", elf->needed.items[i].str);
			}
		}
		return 0;
	}

	lines = ecalloc(1, sizeof(struct slist_t));
	libdirs = init_sysdirs();

	for (elf = head; elf; elf = elf->next) {

		for (i = 0; i < elf->rpaths.i; ++i) {
			libdirs = add_rpath(elf->rpaths.items[i].str, libdirs);
		}

		for (i = 0; i < elf->needed.i; ++i) {
			char *line = NULL;
			size_t len = 0;
			bool match;
			const char *lib = elf->needed.items[i].str;
			char dep_lib[PATH_MAX];
			const char *dep_pkg = NULL;

			match = find_needed(lib, dep_lib, elf, libdirs); 

			if (match && only_missing)
				continue;
			if (match && find_dep_pkg) {
				const struct slist_item_t *dep;
				if ((dep = bsearch_slist(dep_lib, fslist))) {
					dep_pkg = dep->aux;
				}
			}
			if (dep_pkg && no_circular && !strcmp(dep_pkg, elf->pkg))
				continue;
			if (who_needs.use) {
				match = false;
				if (dep_pkg)
					match = !regexec(&who_needs.rex, dep_pkg, 0, NULL, 0);
				if (!match)
					match = !regexec(&who_needs.rex, dep_lib, 0, NULL, 0);
				if (!match)
					continue;
			}
			if (!dep_pkg)
				dep_pkg = missing_str;

			if (verbose & PR_OWN_PACK)
				add_tok_to_line(&line, &len, elf->pkg);
			if (verbose & PR_OWN_PATH)
				add_tok_to_line(&line, &len, elf->path);
			if (verbose & PR_DTNEEDED)
				add_tok_to_line(&line, &len, lib);
			if (verbose & PR_DEP_PATH)
				add_tok_to_line(&line, &len, dep_lib);
			if (verbose & PR_DEP_PACK)
				add_tok_to_line(&line, &len, dep_pkg);

			if (line && len) {
				line[len - 1] = '\0';
				add_to_slist(lines, line, NULL, 1000);
			} else {
				/* canthappen */
				free(line);
			}
		}
	}

	sort_slist(lines);

	for (i = 0; i < lines->i; ++i) {
		char *line = lines->items[i].str;

		while (i + 1 < lines->i && !strcmp(line, lines->items[i + 1].str))
			++i;
		puts(line);
	}

	cleanup_libdir(libdirs);
	cleanup_slist(lines);
	free(lines);

	return err;
}


static char *
parse_rpath(const char *rpath, const char *elfpath) {
	char path[PATH_MAX];
	char *c;
	char *respath;

	memset(path, '\0', PATH_MAX);

	if (strstr(rpath, "$ORIGIN") == rpath) {
		strncpy(path, elfpath, PATH_MAX - 1);

		if (!(c = strrchr(path, '/'))) {
			strcpy(path, "./");
			c = path + 1;
		}

		strncpy(c, rpath + strlen("$ORIGIN"), PATH_MAX - (c - path) - 1);
	} else {
		strncpy(path, rpath, PATH_MAX - 1);
	}

	if (!(respath = realpath(path, NULL))) {
		fprintf(stderr, "%s: %s: could not resolve rpath %s: %s\n", argv0, elfpath, path,
			strerror(errno));
		return NULL;
	}
	strncpy(path, respath, PATH_MAX - 1);
	free(respath);

	return strdup(path);
}

static Elf64_Phdr *
get_proghdrs(FILE *fp, Elf64_Ehdr *elffhdr) {
	Elf64_Phdr *ret;
	int n;

	n = elffhdr->e_phnum;
	ret = emalloc(n * sizeof(Elf64_Phdr));

	if (fseek(fp, elffhdr->e_phoff, SEEK_SET) < 0
	 || elfrw_read_Phdrs(fp, ret, n) != n) {
		free(ret);
		return NULL;
	}

	return ret;
}

static size_t
get_dyns(FILE *fp, Elf64_Phdr *proghdr, size_t n_phdrs, Elf64_Dyn **ret_dyns, char **ret_nmstr) {
	size_t strtab = 0, strsz = 0;
	Elf64_Dyn *dyns;
	size_t n_dyns, i;
	char *nmstr;

	for (i = 0; i < n_phdrs; ++i)
		if (proghdr[i].p_type == PT_DYNAMIC)
			break;
	if (i == n_phdrs)
		return 0;

	if (fseek(fp, proghdr[i].p_offset, SEEK_SET))
		return 0;

	if (is64bit_form())
		n_dyns = proghdr[i].p_filesz / sizeof(Elf64_Dyn);
	else
		n_dyns = proghdr[i].p_filesz / sizeof(Elf32_Dyn);
	dyns = emalloc(n_dyns * sizeof(Elf64_Dyn));

	for (i = 0; i < n_dyns; ++i) {
		if (is64bit_form()) {
			if (1 != fread(&dyns[i], sizeof(Elf64_Dyn), 1, fp))
				break;
			if (!native_form()) {
				revinplc_64xword(&dyns[i].d_tag);
				revinplc_64xword(&dyns[i].d_un);
			}
		} else {
			Elf32_Dyn in32;
			if (1 != fread(&in32, sizeof(Elf32_Dyn), 1, fp))
				break;
			if (native_form()) {
				dyns[i].d_tag = in32.d_tag;
				dyns[i].d_un.d_val = in32.d_un.d_val;
			} else {
				dyns[i].d_tag = rev_32word(in32.d_tag);
				dyns[i].d_un.d_val = rev_32word(in32.d_un.d_val);
			}
		}
		if (dyns[i].d_tag == DT_STRTAB)
			strtab = dyns[i].d_un.d_ptr;
		else if (dyns[i].d_tag == DT_STRSZ)
			strsz = dyns[i].d_un.d_val;
	}

	if (!strtab || !strsz)
		goto free_dyns;
	for (i = 0; i < n_phdrs; ++i)
		if (strtab >= proghdr[i].p_vaddr && strtab < proghdr[i].p_vaddr + proghdr[i].p_filesz)
			break;
	if (i == n_phdrs)
		goto free_dyns;
	if (!(nmstr = getarea(fp, proghdr[i].p_offset + (strtab - proghdr[i].p_vaddr), strsz)))
		goto free_dyns;

	*ret_dyns = dyns;
	*ret_nmstr = nmstr;
	return n_dyns;
free_dyns:
	free(dyns);
	*ret_dyns = NULL;
	*ret_nmstr = NULL;
	return 0;
}


static struct elffile_t*
mk_elffile(const char *path) {
	struct elffile_t *elf = NULL;
	struct stat fs;
	Elf64_Ehdr elffhdr;
	Elf64_Phdr *proghdr;
	FILE *fp;
	Elf64_Dyn *dyns;
	size_t n_dyns, i;
	char *nmstr;

	if (stat(path, &fs) < 0) {
		fprintf(stderr, "%s: ERROR: stat %s: %s\n", argv0, path, strerror(errno));
		return NULL;
	}
	if ((fs.st_mode & S_IXUSR) == 0) {
		return NULL;
	}
	if (!(fp = fopen(path, "rb"))) {
		fprintf(stderr, "%s: ERROR: fopen %s: %s\n", argv0, path, strerror(errno));
		return NULL;
	}
	if (readelfhdr(fp, &elffhdr) < 0 || !(proghdr = get_proghdrs(fp, &elffhdr))) {
		fclose(fp);
		return NULL;
	}

	n_dyns = get_dyns(fp, proghdr, elffhdr.e_phnum, &dyns, &nmstr);

	if (n_dyns > 0 && dyns && nmstr) {
		elf = (struct elffile_t*) ecalloc(1, sizeof(struct elffile_t));
		elf->ec = elffhdr.e_ident[EI_CLASS] == ELFCLASS32 ? EC_32 : EC_64;
		elf->path = strdup(path);

		for (i = 0; i < n_dyns; ++i) {
			char *str, *tok;
			switch (dyns[i].d_tag) {
			case DT_RPATH:
			case DT_RUNPATH:
				str = strdup(nmstr + dyns[i].d_un.d_val);
				tok = strtok(str, ":");

				if (!tok)
					tok = str;

				do {
					char *abs;
					if ((abs  = parse_rpath(tok, elf->path))) {
						add_to_slist(&elf->rpaths, abs, NULL, 100);
					}
				} while ((tok = strtok(NULL, ":")));

				free(str);
				break;
			case DT_NEEDED:
				str = strdup(nmstr + dyns[i].d_un.d_val);
				add_to_slist(&elf->needed, str, NULL, 100);
				break;
			}
		}
		sort_slist(&elf->rpaths);
		sort_slist(&elf->needed);
	}

	fclose(fp);
	free(proghdr);
	free(nmstr);
	free(dyns);

	return elf;
}


static int
mcn(const char *path, dev_t rootdev, bool follow_syms) {
	struct stat fs;
	int err = 0;

	if (follow_syms &&  stat(path, &fs) < 0
	|| !follow_syms && lstat(path, &fs) < 0) {
		fprintf(stderr, "%s: ERROR: stat %s: %s\n", argv0, path, strerror(errno));
		return 1;
	}

	if (S_ISREG(fs.st_mode)) {
		mode_t mode = fs.st_mode & ~S_IFMT;
		mode_t exec = S_IXUSR | S_IXGRP | S_IXOTH;

		if (mode & exec) {
			struct elffile_t *tmp = mk_elffile(path);
			if (tmp) {
				tmp->next = head;
				head = tmp;
			}
		} else {
			return 0;
		}
	}
	else if (S_ISDIR(fs.st_mode) && recurse) {
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

			err |= mcn(fullpath, rootdev, false);
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
mcnx(const char *path) {
	struct stat fs;

	if (stat(path, &fs) == -1) {
		fprintf(stderr, "%s: ERROR: stat %s: %s\n", argv0, path, strerror(errno));
		return -1;
	}

	return mcn(path, fs.st_dev, true);
}


static void
usage() {
	printf("usage: %s [options] <[-r] file ... | -p [pkg ...]>\n", argv0);
	printf("options:\n"
	       "\t-p search for content of pkg(s) rather than file(s)\n"
	       "\t-r recursively search directories on command line\n"
	       "\t-m only print items with unsatisfied dependancies\n"
	       "\t-c omit circular package dependancies\n"
	       "\t-w regex, print only items who has a dependancy matching regex\n"
	);
	printf("output options (may be combined, latest directive has priority):\n"
	       "\t-E print nothing but elf DT_RPATH and DT_NEEDED\n"
	       "\t-o|-O print / omit owning package\n"
	       "\t-f|-F print / omit file path\n"
	       "\t-n|-N print / omit needed library\n"
	       "\t-l|-L print / omit library path\n"
	       "\t-d|-D print / omit dependant package\n"
	       "\t-z alias for -pcEod\n"
	       "\t-v enable all prints\n"
	);
	printf("Defualt output options are: -OfnlD\n");
	printf("Output format is:\n"
	       "\t<owning package>:<file path>:<needed>:<lib path>:<lib package>\n"
	       "\tIf a token should be omitted, the corresponding colon is also omitted.\n"
	       "\tThus, the default output format is:\n"
	       "\t<file path>:<needed>:<lib path>\n"
	);
	       
	exit(1);
}

int
main(int argc, char *argv[]) {
	const char *adm_dir = "/var/log/packages";
	struct slist_t *fslist = NULL;
	bool search_pkgs = false;
	size_t i;
	int err = 0;

	verbose = PR_OWN_PATH | PR_DTNEEDED | PR_DEP_PATH;

	ARGBEGIN {
	case 'A':
		adm_dir = EARGF(usage());
		break;

	case 'p':
		search_pkgs = true;
		find_own_pkg = true;
		break;

	case 'm':
		only_missing = true;
		break;

	case 'x':
	case 'r':
		recurse = true;
		break;

	case 'c':
		find_dep_pkg = true;
		no_circular = true;
		break;

	case 'W':
		find_dep_pkg = true;
		/* FALLTHOROUGH */
	case 'w':
		who_needs.use = true;
		who_needs.str = EARGF(usage());
		break;

	case 'o':
		verbose |=  PR_OWN_PACK;
		break;
	case 'O':
		verbose &= ~PR_OWN_PACK;
		break;
	case 'f':
		verbose |=  PR_OWN_PATH;
		break;
	case 'F':
		verbose &= ~PR_OWN_PATH;
		break;
	case 'n':
		verbose |=  PR_DTNEEDED;
		break;
	case 'N':
		verbose &= ~PR_DTNEEDED;
		break;
	case 'l':
		verbose |=  PR_DEP_PATH;
		break;
	case 'L':
		verbose &= ~PR_DEP_PATH;
		break;
	case 'd':
		verbose |=  PR_DEP_PACK;
		break;
	case 'D':
		verbose &= ~PR_DEP_PACK;
		break;

	case 'q':
	case 'E':
	case 'e':
		verbose = PR_ELF_ONLY;
		break;
	case 'z':
		search_pkgs = true;
		find_own_pkg = true;
		find_dep_pkg = true;
		no_circular = true;
		verbose = PR_OWN_PACK | PR_DEP_PACK;
		break;
	case 'v':
		verbose = ~PR_ELF_ONLY;
		break;


	default:
		usage();
	}
	ARGEND;

	if (!argc && !find_own_pkg)
		usage();

	if (find_own_pkg && recurse)
		usage();

	if (who_needs.use && regcomp(&who_needs.rex, who_needs.str, REG_EXTENDED | REG_NOSUB)) {
		fprintf(stderr, "%s: ERROR: Invalid regex: %s\n", argv0, who_needs.str);
		exit(1);
	}

	if (verbose & PR_DEP_PACK)
		find_dep_pkg = true;
	if (verbose & PR_OWN_PACK)
		find_own_pkg = true;

	if (find_dep_pkg || find_own_pkg) {
		find_dep_pkg = true;
		fslist = (struct slist_t*)ecalloc(1, sizeof(struct slist_t));
		err |= read_adm_dir(adm_dir, NULL, fslist, 0, NULL);

		sort_slist(fslist);
	}

	if (search_pkgs) {
		struct slist_t pkgs;

		memset(&pkgs, 0, sizeof(pkgs));

		err |= read_adm_dir(adm_dir, NULL, &pkgs, argc, argv);

		for (i = 0; i < pkgs.i; ++i) {
			const char *path = pkgs.items[i].str;
			const char *pkg = pkgs.items[i].aux;
			if (!access(path, F_OK)) {
				struct elffile_t *tmp = mk_elffile(path);
				if (tmp) {
					tmp->pkg = strdup(pkg);
					tmp->next = head;
					head = tmp;
				}
			}
		}
		cleanup_slist(&pkgs);
	} else {
		int j;

		for (j = 0; j < argc; ++j) {
			err |= mcnx(argv[j]);
		}

		if (find_dep_pkg || find_own_pkg) {
			struct elffile_t *tmp;

			for (tmp = head; tmp; tmp = tmp->next) {
				const struct slist_item_t *match;
				match = bsearch_slist(tmp->path, fslist);
				if (match) {
					tmp->pkg = strdup(match->aux);
				} else {
					tmp->pkg = strdup(missing_str);
				}
			}
		}
	}

	err |= handle(head, fslist);

	while (head) {
		struct elffile_t *tmp = head->next;

		if (head->pkg)
			free(head->pkg);
		free(head->path);
		cleanup_slist(&head->needed);
		cleanup_slist(&head->rpaths);
		free(head);
		head = tmp;
	}

	if (fslist) {
		cleanup_slist(fslist);
		free(fslist);
	}

	if (who_needs.use) {
		regfree(&who_needs.rex);
	}

	return err ? 1 : 0;
}
