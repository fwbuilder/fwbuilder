#!/bin/sh

if [ -x /bin/bash ];           then BASH="/bin/bash";           fi
if [ -x /usr/bin/bash ];       then BASH="/usr/bin/bash";       fi
if [ -x /usr/local/bin/bash ]; then BASH="/usr/local/bin/bash"; fi
if [ -z "$BASH" ]; then
  echo "Could not find bash."
  exit 1
fi


PACKAGE_NAME="libfwbuilder"
VERSION=`./printversion.sh`
RPMROOT=/var/tmp/${PACKAGE_NAME}
EXPORTDIR=`pwd`/export
TAR_FILE=${PACKAGE_NAME}-${VERSION}.tar.gz
SNAPSHOT_DIR=${PACKAGE_NAME}-${VERSION}

TMP_FILE=${EXPORTDIR}/.maketar.tmp.sh

SPECFILE=`ls *.spec 2>/dev/null`

#cat > ${TMP_FILE} << EOF

function die {
   echo $1
   exit 1
}

if test -z ${PACKAGE_NAME} -o -z ${VERSION}; then
  echo Usage: maketar.sh package_name version
  exit 1
fi


mkdir -p ${EXPORTDIR}

# make distclean

RPMFILES="acconfig.h               \
          acinclude.m4             \
          acsite.m4                \
          autogen.sh               \
          configure                \
          configure.in             \
	  config.sub               \
          config.guess             \
          install.sh               \
          libfwbuilder.pro         \
          maketar.sh               \
          missing                  \
          mkinstalldirs            \
          printversion.sh          \
          qmake.inc                \
          qmake.inc.in             \
          runqmake.sh              \
          VERSION                  \

	  doc                      \
	  etc                      \

	  migration/*.pro          \
          migration/*_[.0-9]*.xslt \

          src/*.pro                \
          src/*/*.in               \
          src/*/*.pro              \
          src/*/*.cpp              \
          src/*/*.h"

RPMFILES="${RPMFILES} ${SPECFILE}"

cd ${EXPORTDIR}
rm -rf ${SNAPSHOT_DIR}
ln -s .. ${SNAPSHOT_DIR}

FILELIST=`for f in ${RPMFILES}; do echo ${SNAPSHOT_DIR}/$f; done`

tar -ch --exclude CVS --exclude *.bak -f - ${FILELIST}  | gzip > ${TAR_FILE} 
cd ..

#EOF
#
#$BASH $TMP_FILE

#rm -f $TMP_FILE


