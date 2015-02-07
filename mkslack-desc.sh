#!/bin/sh
# Copyright (C) 2013 Lars Lindqvist
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.

if [ $# -ne 2 ];then
	printf "usage: `basename $0` <package name> <short package descrition>\n" > /dev/fd/2
	printf "Long description is read from stdin\n" > /dev/fd/2
	exit 1
fi

PKG=$1
DESC=$2
LEN=`printf " $PKG ($DESC)" | wc -c`
if [ $LEN -gt 71 ];then
	printf "`basename $0`: ERROR: short-description too long ($LEN bytes, max=71)\n"
	exit 1
fi
OUT=`mktemp`
fmt -g 71 -w 71 -s < /dev/fd/0 | awk -vpkgname="$PKG" -vpkgdesc="$DESC" '
BEGIN {
	printf("%s: %s (%s)\n%s:\n", pkgname, pkgname, pkgdesc, pkgname);
	lines = 2;
}
{
	++lines;
	printf("%s:", pkgname);
	if(NF > 0) {
		printf(" %s\n", $0);
	} else {
		printf("\n");
	}
}
END {
	if(lines > 11) {
		exit 1
	}
	for(i = lines; i < 11; ++i)
		printf("%s:\n", pkgname);
	exit 0
}' > $OUT
if [ $? -ne 0 ];then
	printf "`basename $0`: ERROR: description too long, max 11 lines\n" 
	nl < $OUT > /dev/fd/2
	rm $OUT
	exit 1
fi
cat $OUT
rm $OUT
exit 0
