#!/bin/sh

test -z "${QMAKE}" && QMAKE="qmake"

C=""
test -n "$QMAKESPEC" && C+="-spec $QMAKESPEC "
test -n "$CCACHE" && C+='QMAKE_CXX="ccache	g++" '

echo "QTDIR=\"$QTDIR\""
echo "Running qmake: $QMAKE $C"

oIFS=$IFS
IFS=" 
"

$QMAKE $C -recursive



# test -n "$CCACHE" && C='QMAKE_CXX="ccache g++"'
# test -n "$QMAKESPEC" && C="-spec $QMAKESPEC $C"

# echo "QTDIR=$QTDIR"
# echo "Running qmake: ${QMAKE} $C"

# oIFS=$IFS
# IFS=
# ${QMAKE} $C
# for d in src/ src/fwbuilder/ src/fwcompiler/ src/test/ \
#          src/confscript/ etc/ doc/ migration/
# do
#   (cd $d; ${QMAKE} $C)
# done
# IFS=$oIFS

