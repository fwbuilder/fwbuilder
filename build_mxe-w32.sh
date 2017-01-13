#!/bin/bash
JOBS=$(nproc --all)

export PATH=/usr/lib/mxe/usr/bin:$PATH
export TOOLCHAIN_PREFIX=/usr/lib/mxe/usr/i686-w64-mingw32.shared

./autogen.sh \
  --with-xml2-config=/usr/lib/mxe/usr/i686-w64-mingw32.shared/bin/xml2-config \
  --with-xslt-config=/usr/lib/mxe/usr/i686-w64-mingw32.shared/bin/xslt-config \
  --with-qtdir=/usr/lib/mxe/usr/i686-w64-mingw32.shared/qt5 \
  --host=i686-w64-mingw32.shared
make -j${JOBS}

makensis -nocd packaging/fwbuilder.nsi
