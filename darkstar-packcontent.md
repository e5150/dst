DARKSTAR-PACKCONTENT(1)          General Commands Manual (darkstar-tools-14.2)

NAME
     darkstar-packcontent – prints content of installed package(s)

SYNOPSIS
     darkstar-packcontent [-v] [-e | -m] package-database-file ...

DESCRIPTION
     darkstar-packcontent prints information about the files belonging to an
     installed package.  It reads the the "FILE LIST:" section of a packge,
     and can find out if a file or directory is still present on the system or
     not, and compute the actual installed size of a package.

OPTIONS
     -v      Verbose output, prepend lines with "<package name>:"

     -e      Print only existing files.

     -m      Print only missing files.

NOTES
     The /install directory is always ignored, since it should never actually
     stay on the file system when a package has been properly installed.

     The ".new" suffix gets stripped from filenames, since install/doinst.sh
     will rename such files on a clean system, might cause false results.

     To accommodate glibc{,-solibs} lib/incoming/ and lib64/incoming/ are
     truncated to lib/ and lib64/, respectively.


SEE ALSE
     darkstar-installed(1),

                              16th of April 2014
