/*
 * Copyright © 2016 Lars Lindqvist <lars.lindqvist at yandex.ru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the “Software”),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "slist.h"
#include "ealloc.h"

void
cleanup_slist(struct slist_t *list) {
	size_t i;

	for (i = 0; i < list->i; ++i) {
		free(list->items[i].str);
		if (list->items[i].aux)
			free(list->items[i].aux);
	}
	free(list->items);
}

static int
cmp_str(const void *a, const void *b) {
/*	return strcmp(*(char *const *)a, *(char *const *)b); */
	return strcmp(((const struct slist_item_t*)a)->str,
	              ((const struct slist_item_t*)b)->str);
}
static int
cmp_siz(const void *a, const void *b) {
	size_t as = ((const struct slist_item_t*)a)->siz;
	size_t bs = ((const struct slist_item_t*)b)->siz;

	return as - bs;
}

void
sort_slist(struct slist_t *list) {
	qsort(list->items, list->i, sizeof(struct slist_item_t), cmp_str);
}

void
sort_slist_siz(struct slist_t *list) {
	qsort(list->items, list->i, sizeof(struct slist_item_t), cmp_siz);
}

void
add_to_slist(struct slist_t *list, char *str, char *aux, size_t incr) {
	if (list->n == list->i) {
		list->n += incr;
		list->items = (struct slist_item_t*)
		              erealloc(list->items, list->n * sizeof(struct slist_item_t));
	}
	list->items[list->i].str = str;
	list->items[list->i].aux = aux;
	list->items[list->i].siz = 0;

	list->i++;
}

static struct slist_item_t *
_bsearch_slist(const char *str, const struct slist_t *list, int search_aux) {
	int min, max, mid;
	int cmp;

	min = 0;
	max = list->i - 1;

	while (min <= max) {
		mid = (min + max) / 2;
		cmp = strcmp(search_aux ? list->items[mid].aux : list->items[mid].str, str);

		if (cmp == 0)
			return list->items + mid;
		else if (cmp < 0)
			min = mid + 1;
		else
			max = mid - 1;
	}
	return NULL;
}

struct slist_item_t *
bsearch_slist(const char *str, const struct slist_t *list) {
	return _bsearch_slist(str, list, 0);
}

struct slist_item_t *
bsearch_slist_aux(const char *str, const struct slist_t *list) {
	return _bsearch_slist(str, list, 1);
}
