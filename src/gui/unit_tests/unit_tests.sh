#!/bin/bash

commands=$@


for directory in `find . -maxdepth 1 -type d -regex '\./[A-Za-z0-9\-\_]*'`
do
    cd $directory
    [ ! -e Makefile ] && qmake -spec $QMAKESPEC

    Xvfb :1 &
    PID=$!
    export DISPLAY=":1"
    $commands || { kill $PID; exit 1; }
    kill $PID || exit 0

    cd -
done

