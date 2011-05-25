#!/bin/sh

QMAKE="${QMAKE:-qmake}"
QMAKEPARAMS="${QMAKESPEC:+ -spec $QMAKESPEC}"

set -e
find . -mindepth 1 -type d -name unit_tests | while read directory
do
	echo "===> Running tests in $directory"
	(cd $directory && $QMAKE $QMAKEPARAMS && "$@")
done
