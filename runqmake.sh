#!/bin/sh

test -z "${QMAKE}" && QMAKE="qmake"

echo "Running qmake: ${QMAKE}"

${QMAKE}
${QMAKE}  -o src/Makefile  src/src.pro

if test -n "$CCACHE"; then
    ${QMAKE}  'QMAKE_CXX=ccache g++' -o src/fwbuilder/Makefile  src/fwbuilder/fwbuilder.pro
    ${QMAKE}  'QMAKE_CXX=ccache g++' -o src/fwcompiler/Makefile src/fwcompiler/fwcompiler.pro
    ${QMAKE}  'QMAKE_CXX=ccache g++' -o src/test/Makefile       src/test/test.pro
else
    ${QMAKE}                         -o src/fwbuilder/Makefile  src/fwbuilder/fwbuilder.pro
    ${QMAKE}                         -o src/fwcompiler/Makefile src/fwcompiler/fwcompiler.pro
    ${QMAKE}                         -o src/test/Makefile       src/test/test.pro
fi

${QMAKE} -o src/confscript/Makefile src/confscript/confscript.pro

${QMAKE} -o etc/Makefile        etc/etc.pro
${QMAKE} -o doc/Makefile        doc/doc.pro
${QMAKE} -o migration/Makefile  migration/migration.pro


