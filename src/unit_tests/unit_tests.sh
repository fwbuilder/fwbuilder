#!/bin/sh

QMAKE="${QMAKE:-qmake}"
QMAKEPARAMS="${QMAKESPEC:+ -spec $QMAKESPEC}"

set -e

build() {
	local _d="$1"
	shift
	(
	    cd "$_d" && $QMAKE $QMAKEPARAMS && "$@"
	)
}

build main "$@"

find . -maxdepth 1 -type d |
    egrep -- '^\./[A-Za-z0-9_-]*$' |
    while read _d
do
	echo "======================= $_d"
	build "$_d" "$@"
done

