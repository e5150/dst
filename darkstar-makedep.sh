#!/bin/sh
# Copyright © 2008-2010, 2014, 2015 Lars Lindqvist
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
	printf "usage: %s [-l] [-f]\n" $argv0 > /dev/fd/2
	exit 1
}

while getopts "flr:" opt; do
case "$opt" in
	f) REHASH=yes ;;
	l) USE_LDD=ldd ;;
	# undocumented
	r) ROOT=$OPTARG ;;
	*) usage ;;
esac
done

if [ `expr $OPTIND - 1` -ne $# ];then
	usage
fi

if [ ! -z $ROOT ] && [ ! -d $ROOT ];then
	printf "%s: ERROR: %s: Not a directory\n" $argv0 $ROOT
	usage
fi


COOKIE=/var/tmp/dst-deps-${USE_LDD:-elfdeps}-`echo $ROOT | sed 's#/#_#g'`

if [ ! -f $COOKIE ];then
	# can't use a non existing cache
	REHASH=yes
elif [ `stat -c %Y $COOKIE` -lt `stat -c %Y /var/log/packages` ]; then
	# cache is old, make new one
	REHASH=yes
fi

if [ "$REHASH" = "yes" ];then
	if [ -z $USE_LDD ];then
		find ${ROOT:-/ /usr /opt} -xdev -type f -perm -100 2>/dev/null \
		| sort \
		| uniq \
		| xargs elfdeps \
		> $COOKIE
	else
		# reformat ldd output, to that of elfdeps
		find ${ROOT:-/ /usr /opt} -xdev -type f -perm -100 2>/dev/null \
		| sort \
		| uniq \
		| xargs ldd \
		| grep -v 'not a dynamic executable'  \
		| sed -e 's/=> //' -e 's/not found/missing/' \
		| awk '{
			if (NF == 1) {
				file = $1;
			} else {
				printf("%s%s:%s\n", file, $1, $2);
			}
		}' \
		| grep -v ':(0x' \
		> $COOKIE
	fi
fi

echo $COOKIE
exit 0
