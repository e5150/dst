DARKSTAR-INSTALLED(1)            General Commands Manual (darkstar-tools-14.2)

NAME
     darkstar-installed – querys the package database for installed packages

SYNOPSIS
     darkstar-installed pkgname ...

DESCRIPTION
     darkstar-installed looks in /var/log/packages for installed packages and
     prints the path to the database file for the corresponding package, if
     found.

ARGUMENTS
     package name may be a package filename, e.g. "nc-1.10-x86_64-1.txz" or a
     package basename, e.g. "xz-5.0.5-x86_64-1", either optionally with a
     leading path, or simply a package name, e.g. "xz" or "glibc-solibs".

EXIT STATUS
     0       All packages are installed

     1       Any package is not installed, or error.


SEE ALSE
     darkstar-packname(1), darkstar-packcontent(1).

                              17th of April 2014
