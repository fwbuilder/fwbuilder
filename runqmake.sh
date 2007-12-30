#!/bin/sh

test -z "${QMAKE}" && QMAKE="qmake"
C=""
test -n "$CCACHE" && C='QMAKE_CXX="ccache g++"'

echo "QTDIR=$QTDIR"
echo "Running qmake: ${QMAKE} ${C}"

oIFS=$IFS
IFS=

${QMAKE} $C

for d in src/ \
    src/res/ \
    src/tools/ \
    doc/ \
    po/ \
    src/antlr/ \
    src/gui/ \
    src/fwblookup/ \
    src/fwbedit/ \
    src/ipt/ \
    src/pflib/ \
    src/pf/ \
    src/ipf/ \
    src/ipfw/ \
    src/parsers/
do
  (cd $d; ${QMAKE} $C)
done

test -d src/unit_tests && {
  (cd src/unit_tests/importer/; ${QMAKE} $C)
}

IFS=$oIFS

