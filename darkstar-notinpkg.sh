#!/bin/sh
# Copyright © 2010,2015 Lars Lindqvist
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

export LC_ALL=C

FILES_PKG=`mktemp /tmp/dst-pckcmp-inst-pck.XXXXXX`
FILES_SYS=`mktemp /tmp/dst-pckcmp-inst-sys.XXXXXX`

# Ignore devs if we have devtmpfs or such
if findmnt /dev >/dev/null 2>&1;then
	IGNOREDEV="grep -v ^/dev"
fi

darkstar-packcontent -d /var/log/packages/* \
| sed 's#/$##' \
| sort \
| uniq \
| ${IGNOREDEV:-cat} \
> $FILES_PKG

find / /usr /opt -mindepth 1 -xdev ! -type l \
| sort \
| uniq \
| grep -v '^/root/' \
| grep -vE '^/var/(log|run|cache|tmp|spool)' \
> $FILES_SYS


case "$0" in
*darkstar-notonsys*) COMMMODE="-32" ;;
*darkstar-notinpkg*)   COMMMODE="-31" ;;
esac

comm $COMMMODE $FILES_PKG $FILES_SYS

rm $FILE_PKG $FILES_SYS
