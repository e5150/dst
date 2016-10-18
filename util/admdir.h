#ifndef _ADMDIR_H_
#define _ADMDIR_H_
#include "slist.h"
int read_adm_pkg (const char *path, struct slist_t *list);
int read_adm_dir (const char *dir, struct slist_t *pkg_list, struct slist_t *content_list, int pkgc, char **pkgv);
#endif
