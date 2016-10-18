ELFDEPS(1)       General Commands Manual (darkstar-tools-14.2)      ELFDEPS(1)

NAME
     elfdeps – querys ELF files for NEEDED libraries.

SYNOPSIS
     elfdeps [-v | -q] [-m] [-r] file ... | -p pkg ...

DESCRIPTION
     elfdeps scans the dynamic section of an ELF file for DT_NEEDED libraries,
     and does a query for the required libraries in the /lib, /usr/lib,
     /usr/local/lib and /lib64, /usr/lib64, /usr/local/lib64 directories for
     32 and 64 bit ELF files, respectively. Any DT_RPATH found in the dynamic
     section is also scanned for the libraries.  Output is formatted as
     "(<pkg>:)<file>:<lib>:<path>(:<pkg>)", where file is the path given as
     argument, lib is the needed library and path is either the path to
     required library or "missing".

ARGUMENTS
     -q Quiet mode, only print DT_NEEDED and DT_RPATHS of file.

     -r Recursively traverse and directories given on command line.  Increase
     verbosity, may be given multiple times, only for debugging.

     -d Print the package (if any) containing the needed library.

     -p pkg ... Search files owned by the given package(s). Basically:

           $ cat /var/log/packages/pkgname | xargs elfdeps

CAVEATS
     elfdeps does not actually resolve library dependencies, it ignores
     /etc/ld.so.cache, /etc/ld.so.conf and $LD_LIBRARY_PATH. It is designed to
     sacrifice accuracy for speed. For proper dependency resolution use
     ldd(1).  The program scans each directory only once, multiple file s
     sharing DT_RPATH, or DT_RPATH containing e.g. /usr/lib64, will not incur
     rescanning of the given directory. Files in a scanned directory is
     regarded as a potential library iff its name contains the string ".so",
     they are never stat(2)'ed, so they might be dangling symlinks, sockets,
     directories, or any non-shared-library file possible. The fact that
     elfdeps claims that a library is either missing or found at some
     particular path does not answer wheter or not a library dependency is
     actually satisfied, but hopefully it should give an accrurate picture in
     most cases.

BUGS
     DT_RPATH is (allegedly) deprecated, DT_RUNPATH should be queried as well,
     but isn't.


SEE ALSE
     ldd(1), readelf(1), objdump(1), elf(5), ld.so(8),

                             19th of October 2016
