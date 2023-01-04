#!/bin/bash

#TARBALL=`pwd`/fwbuilder-${VERSION}.tar.gz
REDHAT_DIR=`pwd`
cd ..
SOURCE_DIR=`pwd`

TMPDIR=`mktemp -d`

# Generate the versioned files and move them back to the source directory
cd ${TMPDIR}
cmake ${SOURCE_DIR}

mv redhat/fwbuilder.spec redhat/mktarball.sh ${REDHAT_DIR}
chmod a+x ${REDHAT_DIR}/mktarball.sh

rm -rf ${TMPDIR}
