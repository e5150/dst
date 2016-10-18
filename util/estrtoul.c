#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern const char *argv0;

unsigned long
estrtoul(const char *s, int base) {
	char *end;
	unsigned long n;

	errno = 0;
	n = strtoul(s, &end, base);
	if (errno != 0) {
		fprintf(stderr, "%s: FATAL: strtoul %s %d: %s\n", argv0, s, base, strerror(errno));
	} else if (*end != '\0') {
		fprintf(stderr, "%s: FATAL: strtoul %s: Not a base %d integer\n", argv0, s, base);
	} else {
		/* all is good */
		return n;
	}
	exit(1);
}
unsigned long eatoul(const char *s) {
	return estrtoul(s, 0);
}
