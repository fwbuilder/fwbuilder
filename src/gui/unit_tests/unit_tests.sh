#!/bin/bash

commands=$@

Xvfb :1 &
PID=$!
export DISPLAY=":1"

for directory in `find . -maxdepth 1 -type d -regex '\./[A-Za-z0-9\-\_]*'`
do
    cd $directory
    [ ! -e Makefile ] && qmake -spec $QMAKESPEC
    $commands
    cd -
done

kill $PID
