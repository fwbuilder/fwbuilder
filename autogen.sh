#!/bin/sh


MAKE=`which gnumake 2>/dev/null`
if test ! -x "$MAKE" ; then MAKE=`which gmake` ; fi
if test ! -x "$MAKE" ; then MAKE=`which make` ; fi
HAVE_GNU_MAKE=`$MAKE --version|grep -c "Free Software Foundation"`

if test "$HAVE_GNU_MAKE" != "1"; then 
  echo "Could not find GNU make on this system, can not proceed with build."
  exit 1
else
  echo "Found GNU Make at $MAKE ... good."
fi

if test ! -x "`which aclocal`"
then
  echo "You need aclocal and autoconf to generate configure and Makefile."
  echo "Please install autoconf package."
  exit 1
fi

if test ! -x "`which libtoolize`"
then
  if test ! -x "`which glibtoolize`"
  then
    echo "You need libtoolize to generate autoconf and libtool scripts."
    echo "Please install libtool package."
    exit 1
  fi
fi

echo "This script runs configure ..."

which libtoolize >/dev/null 2>&1 && libtoolize --force --copy
which glibtoolize >/dev/null 2>&1 && glibtoolize --force --copy
which acinclude >/dev/null 2>&1 && acinclude
which aclocal >/dev/null 2>&1 && aclocal
autoconf

./configure ${CFGARGS}  $*
