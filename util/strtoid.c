#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>

gid_t
strtogid(const char *str) {
	struct group *gr = NULL;

	errno = 0;
	gr = getgrnam(str);
	if (errno) {
		return (gid_t)-1;
	} else if (!gr) {
		errno = EINVAL;
		return (gid_t)-1;
	}
	return gr->gr_gid;
}

uid_t
strtouid(const char *str) {
	struct passwd *pw = NULL;

	errno = 0;
	pw = getpwnam(str);
	if (errno) {
		return (uid_t)-1;
	} else if (!pw) {
		errno = EINVAL;
		return (uid_t)-1;
	}
	return pw->pw_uid;
}
