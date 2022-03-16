#!/bin/bash

VERSION=6.0.0

TMPDIR=`mktemp -d`
DESTDIR=${TMPDIR}/fwbuilder-${VERSION}

TARBALL=`pwd`/fwbuilder-${VERSION}.tar.bz2

# Make a copy of the source with the proper version number
cd ..
find . -depth -print0 | cpio --null -pdm ${DESTDIR}

# Clean up any unwanted files
cd ${DESTDIR}
rm -rf .git*

cd ..
tar -cjf ${TARBALL} *

rm -rf ${TMPDIR}
