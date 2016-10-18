#ifndef _SLIST_H_
#define _SLIST_H_
struct slist_item_t {
	char *str;
	char *aux;
	size_t siz;
};

struct slist_t {
	struct slist_item_t *items;
	size_t n;
	size_t i;
};

struct slist_item_t * bsearch_slist(const char *item,const struct slist_t *list);
struct slist_item_t * bsearch_slist_aux(const char *item,const struct slist_t *list);
void add_to_slist(struct slist_t *list,char *str,char *aux,size_t incr);
void sort_slist(struct slist_t *list);
void sort_slist_siz(struct slist_t *list);
void cleanup_slist(struct slist_t *list);
#endif
