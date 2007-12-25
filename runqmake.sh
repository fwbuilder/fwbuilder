#!/bin/sh

if test -f qtdir ; then
  QTDIR=`cat qtdir`
  export QTDIR
fi

test -z "${QMAKE}" && QMAKE="qmake"

echo "Running qmake: ${QMAKE}"

${QMAKE}

${QMAKE} -o src/Makefile        src/src.pro
${QMAKE} -o src/res/Makefile    src/res/res.pro
${QMAKE} -o src/tools/Makefile  src/tools/tools.pro
${QMAKE} -o doc/Makefile        doc/doc.pro
${QMAKE} -o po/Makefile         po/po.pro

if test -n "$CCACHE"; then

${QMAKE} 'QMAKE_CXX=ccache g++'  -o src/antlr/Makefile     src/antlr/antlr.pro

${QMAKE} 'QMAKE_CXX=ccache g++'  -o src/gui/Makefile       src/gui/gui.pro
${QMAKE} 'QMAKE_CXX=ccache g++'  -o src/fwblookup/Makefile src/fwblookup/fwblookup.pro
${QMAKE} 'QMAKE_CXX=ccache g++'  -o src/fwbedit/Makefile   src/fwbedit/fwbedit.pro
${QMAKE} 'QMAKE_CXX=ccache g++'  -o src/ipt/Makefile       src/ipt/ipt.pro
${QMAKE} 'QMAKE_CXX=ccache g++'  -o src/pflib/Makefile     src/pflib/pflib.pro
${QMAKE} 'QMAKE_CXX=ccache g++'  -o src/pf/Makefile        src/pf/pf.pro
${QMAKE} 'QMAKE_CXX=ccache g++'  -o src/ipf/Makefile       src/ipf/ipf.pro
${QMAKE} 'QMAKE_CXX=ccache g++'  -o src/ipfw/Makefile      src/ipfw/ipfw.pro

${QMAKE} 'QMAKE_CXX=ccache g++'  -o src/parsers/Makefile   src/parsers/parsers.pro

test -d src/unit_tests && ${QMAKE} 'QMAKE_CXX=ccache g++'  -o src/unit_tests/importer/Makefile  \
src/unit_tests/importer/importer.pro

else

${QMAKE}  -o src/antlr/Makefile     src/antlr/antlr.pro

${QMAKE}  -o src/gui/Makefile       src/gui/gui.pro
${QMAKE}  -o src/fwblookup/Makefile src/fwblookup/fwblookup.pro
${QMAKE}  -o src/fwbedit/Makefile   src/fwbedit/fwbedit.pro
${QMAKE}  -o src/ipt/Makefile       src/ipt/ipt.pro
${QMAKE}  -o src/pflib/Makefile     src/pflib/pflib.pro
${QMAKE}  -o src/pf/Makefile        src/pf/pf.pro
${QMAKE}  -o src/ipf/Makefile       src/ipf/ipf.pro
${QMAKE}  -o src/ipfw/Makefile      src/ipfw/ipfw.pro

${QMAKE}  -o src/parsers/Makefile   src/parsers/parsers.pro

test -d src/unit_tests && ${QMAKE} -o src/unit_tests/importer/Makefile \
src/unit_tests/importer/importer.pro

fi

