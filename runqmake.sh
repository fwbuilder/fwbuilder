#!/bin/sh

test -z "${QMAKE}" && QMAKE="qmake"
C=""
test -n "$QMAKESPEC" && C="$C -spec $QMAKESPEC "
test -n "$CCACHE" && C="$C QMAKE_CXX=\"ccache	g++\" "

echo "QTDIR=\"$QTDIR\""
echo "Running qmake: $QMAKE $C"

oIFS=$IFS
IFS=" 
"

$QMAKE $C -recursive

