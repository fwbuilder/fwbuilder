#!/bin/sh

test -z "${QMAKE}" && QMAKE="qmake"
C="-recursive"
test -n "$QMAKESPEC" && C="$C -spec $QMAKESPEC "

echo "QTDIR=\"$QTDIR\""
echo "Running qmake: $QMAKE $C"

oIFS=$IFS
IFS=" 
"

$QMAKE $C

