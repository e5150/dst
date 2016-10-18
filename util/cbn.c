#include <string.h>

char *
cbn(const char *path) {
	char *bn = strrchr(path, '/');

	if (bn)
		return ++bn;
	else
		return (char*)path;
}
