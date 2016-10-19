DARKSTAR-PACKNAME(1)             General Commands Manual (darkstar-tools-14.2)

NAME
     darkstar-packname – extract package information from filename

SYNOPSIS
     darkstar-packname [-v | -b] filename ...

DESCRIPTION
     darkstar-packname extracts package information from slackware package
     filenames that adheres to the <name>-<version>-<arch>-<build><tag>[.ext]
     format, where ext is one of tgz, txz, tbz or tlz, and only the name part
     may contain a '-'.  Any leading path is ignored.

OPTIONS
     -b      Print package basename, without extension.

     -v      Print package version


SEE ALSE
     darkstar-installed(1),

                             19th of October 2016
