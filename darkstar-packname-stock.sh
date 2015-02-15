#!/bin/sh
# Copyright 1994, 1998, 2000  Patrick Volkerding, Concord, CA, USA 
# Copyright 2001, 2003  Slackware Linux, Inc., Concord, CA, USA
# Copyright 2007, 2009, 2011  Patrick Volkerding, Sebeka, MN, USA 
# All rights reserved.
#
# Redistribution and use of this script, with or without modification, is
# permitted provided that the following conditions are met:
#
# 1. Redistributions of this script must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
#  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
#  EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

# installpkg, stripped of everything but the package_name related stuff

pkgbase() {
  PKGEXT=$(echo $1 | rev | cut -f 1 -d . | rev)
  case $PKGEXT in
  'tgz' )
    PKGRETURN=$(basename $1 .tgz)
    ;;
  'tbz' )
    PKGRETURN=$(basename $1 .tbz)
    ;;
  'tlz' )
    PKGRETURN=$(basename $1 .tlz)
    ;;
  'txz' )
    PKGRETURN=$(basename $1 .txz)
    ;;
  *)
    PKGRETURN=$(basename $1)
    ;;
  esac
  echo $PKGRETURN
}


package_name() {
  STRING=$(pkgbase $1)
  # Check for old style package name with one segment:
  if [ "$(echo $STRING | cut -f 1 -d -)" = "$(echo $STRING | cut -f 2 -d -)" ]; then
    echo $STRING
  else # has more than one dash delimited segment
    # Count number of segments:
    INDEX=1
    while [ ! "$(echo $STRING | cut -f $INDEX -d -)" = "" ]; do
      INDEX=$(expr $INDEX + 1)
    done
    INDEX=$(expr $INDEX - 1) # don't include the null value
    # If we don't have four segments, return the old-style (or out of spec) package name:
    if [ "$INDEX" = "2" -o "$INDEX" = "3" ]; then
      echo $STRING
    else # we have four or more segments, so we'll consider this a new-style name:
      NAME=$(expr $INDEX - 3)
      NAME="$(echo $STRING | cut -f 1-$NAME -d -)"
      echo $NAME
      # cruft for later ;)
      #VER=$(expr $INDEX - 2)
      #VER="$(echo $STRING | cut -f $VER -d -)"
      #ARCH=$(expr $INDEX - 1)
      #ARCH="$(echo $STRING | cut -f $ARCH -d -)"
      #BUILD="$(echo $STRING | cut -f $INDEX -d -)"
    fi
  fi
}

# Main loop:
for package in $* ; do
  package_name $package
done
