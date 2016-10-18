DARKSTAR-PACKSIZE(1)             General Commands Manual (darkstar-tools-14.2)

NAME
     darkstar-packsize – prints installed size of package(s)

SYNOPSIS
     darkstar-packsize [-v] [-S | -t | -l] [pkgname ...]

DESCRIPTION
     darkstar-packsize prints the size of package content in human readable
     form. It might differ from the UNCOMPRESSED PACKAGE SIZE reported in the
     package database, if files from a package has been removed. If no
     arguments are given the size of all installed packages will be listed, in
     ascending order of the package size.

OPTIONS
     -S      Do not sort output.

     -t      Sort by ascending installation time

     -l      Print the last few packages, by installation time

                             16th of October 2016
