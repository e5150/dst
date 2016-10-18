#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

extern const char *argv0;

void *
erealloc(void *p, size_t size) {
	p = realloc(p, size);
	if (!p) {
		fprintf(stderr, "%s: FATAL: realloc %lu: %s\n", argv0, size, strerror(errno));
		exit(1);
	}
	return p;
}

void *
ecalloc(size_t nmemb, size_t size) {
	void *p = calloc(nmemb, size);
	if (!p) {
		fprintf(stderr, "%s: FATAL: calloc %lu %lu: %s\n", argv0, nmemb, size, strerror(errno));
		exit(1);
	}
	return p;
}

void *
emalloc(size_t size) {
	void *p = malloc(size);
	if (!p) {
		fprintf(stderr, "%s: FATAL: malloc %lu: %s\n", argv0, size, strerror(errno));
		exit(1);
	}
	return p;
}
