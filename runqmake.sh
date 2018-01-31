#!/bin/sh

test -z "${QMAKE}" && QMAKE="qmake"
C="-recursive"
test -n "$QMAKESPEC" && C="$C -spec $QMAKESPEC "

echo "QTDIR=\"$QTDIR\""
echo "Running qmake: $QMAKE $C"

oIFS=$IFS
IFS=" 
"

test -z "$EXTRA_CXXFLAGS" || FLAGS="QMAKE_CXXFLAGS += $EXTRA_CXXFLAGS"

if [ -z "$FLAGS" ]; then
  # in case there are no FLAGS defined, we can't pass an empty "" parameter to qmake
  $QMAKE $C
else
  $QMAKE "$FLAGS" $C
fi

