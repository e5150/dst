#!/bin/sh
# Copyright © 2011 Lars Lindqvist
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


TMP=${TMP:-/tmp}

argv0=`basename $0`

usage() {
	printf "usage: %s [-l] [-f] <regex>\n" $argv0 > /dev/fd/2
	exit 1
}

while getopts "pflr:" opt; do
case "$opt" in
	f) export REHASH=yes ;;
	l) export USE_LDD=ldd ;;
	# undocumented
	r) export ROOT=$OPTARG ;;
	*) usage ;;
esac
done

if [ $OPTIND -ne $# ];then
	usage
fi
shift `expr $OPTIND - 1`

cat `darkstar-makedep` \
| awk -F: '{printf("%s:%s\n", $2, $1)}' \
| sort \
| egrep "^$1"
