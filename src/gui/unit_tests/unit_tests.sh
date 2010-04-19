#!/bin/bash

commands=$@


for directory in `find . -maxdepth 1 -type d -regex '\./[A-Za-z0-9\-\_]*'`
do
    cd $directory
    [ ! -e Makefile ] && qmake -spec $QMAKESPEC

    PID=""
    [ -z "$DISPLAY" ] && {
        Xvfb :1 &
        PID=$!
        export DISPLAY=":1"
    }
    $commands; TEST_STATUS=$?
    [ -n "$PID" ] && kill $PID
    [ $TEST_STATUS ] || exit 0

    cd -
done

