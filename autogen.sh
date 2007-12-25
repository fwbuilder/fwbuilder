#!/bin/sh


MAKE=`which gnumake 2>/dev/null`
if test ! -x "$MAKE" ; then MAKE=`which gmake` ; fi
if test ! -x "$MAKE" ; then MAKE=`which make` ; fi
HAVE_GNU_MAKE=`$MAKE --version|grep -c "Free Software Foundation"`

if test "$HAVE_GNU_MAKE" != "1"; then 
  echo Could not find GNU make on this system, can not proceed with build.
  exit 1
else
  echo Found GNU Make at $MAKE ... good.
fi

echo This script runs configure ...
echo You did remember necessary arguments for configure, right?

if test ! -x "`which aclocal`"
then echo you need autoconfig to generate the Makefile
fi

sys=`uname -s`

libtoolize --force --copy
# autoheader
acinclude
aclocal
autoconf

./configure ${CFGARGS}  $*
