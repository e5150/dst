CUFGF(1)         General Commands Manual (darkstar-tools-14.2)        CUFGF(1)

NAME
     cugfd – change user/group/file/directory ownership and permissions

SYNOPSIS
     cugfd [-KDql] [-u user] [-g group] [-f file-mode] [-d dir-mode] path ...



DESCRIPTION
     cugfd recursively changes ownership and/or permissions on files and
     directories.  At least one of -u, -g, -f or -d must be given for any
     changes to be made, if either option is not given, the corresponding
     owership/permissions will be left unchanged.

     By default, the executable bit will be kept for those with read access.

OPTIONS
     -u user
             Change user ownership to user.

     -g group
             Change group ownership to group.

     -f file-mode
             Change mode of regular files to the octal file-mode.

     -d dir-mode
             Change mode of directories to the octal dir-mode.

     -l      Follow symbolic links.

     -U      Only handle unique files (with no hardlinks).

     -x      Don't recurse into mountpoints.

     -K      Don't keep executable bit on regular files.

     -D      Dry run, don't do any changes.

     -q      Quiet operation, only print warnings and errors.


SEE ALSE
     chmod(1), chown(1),

                             9th of September 2013
